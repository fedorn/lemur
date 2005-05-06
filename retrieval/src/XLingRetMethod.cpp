/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */


#include "XLingRetMethod.hpp"
#include "DocInfoList.hpp"
#include "SimpleKLRetMethod.hpp"
#include <cmath>

using namespace lemur::api;

lemur::retrieval::XLingRetMethod::XLingRetMethod(const Index &dbIndex,
                                                 const Index &background,
                                                 lemur::dictionary::PDict &dict,
                                                 ScoreAccumulator &accumulator,
                                                 double l, double b, 
                                                 bool cacheDR,
                                                 string &sBM, string &tBM,
                                                 const Stopper *stp, 
                                                 Stemmer *stm) : 
  RetrievalMethod(dbIndex), scAcc(accumulator), source(background),
  dictionary(dict), cacheDocReps(cacheDR), lambda(l), beta(b), 
  stopper(stp), stemmer(stm) {

  docBasedSourceSmooth = (sBM == "doc");
  docBasedTargetSmooth = (tBM == "doc");
  numSource = 0;
  if (docBasedSourceSmooth) {
    for (TERMID_T i = 1; i < source.termCountUnique(); i++) 
      numSource += source.docCount(i);
  } else {
    numSource = source.termCount(); 
  }
  numTarget = 0;
  if (docBasedTargetSmooth) {
    for (TERMID_T i = 1; i < ind.termCountUnique(); i++) 
      numTarget += ind.docCount(i);
  } else {
    numTarget = ind.termCount(); 
  }

  if (cacheDocReps) {
    COUNT_T dc = ind.docCount();
    docRepsSize = dc + 1;
    docReps = new DocumentRep *[docRepsSize];
    for (DOCID_T i = 0; i <= dc; i++) docReps[i] = NULL;
  }
  termScores = new ArrayAccumulator(ind.docCount());
}

lemur::api::QueryRep *lemur::retrieval::XLingRetMethod::computeTargetKLRep(const QueryRep *qry) {
  // should not have to cast.
  const XLingQueryModel *qm = dynamic_cast<const XLingQueryModel *>(qry);
  SimpleKLQueryModel *qr = new SimpleKLQueryModel(ind);

  qm->startIteration();
  while (qm->hasMore()) {
    const XLQueryTerm &qt = qm->nextTerm();
    double sourceWeight = qt.weight();
    lemur::dictionary::DictEntryVector *xlates = qt.getTranslations();
    if (xlates != NULL) {
      for (lemur::dictionary::DictEntryVector::iterator it = xlates->begin(); 
           it != xlates->end(); it++) {
        TERMID_T tid = ind.term((*it).target);
        if (tid > 0) {
          double prob = (*it).prob;
          qr->incCount(tid, prob * sourceWeight);
        }
      }
      delete(xlates);
    }
  }
  // update value of colQLikelihood.
  qr->colQueryLikelihood();
  return qr;
}


lemur::retrieval::XLingRetMethod::~XLingRetMethod() {
  if (cacheDocReps) {
    for (DOCID_T i = 0; i < docRepsSize; i++) delete(docReps[i]);
    delete[](docReps);
  }
  delete(termScores);
}

lemur::api::DocumentRep *lemur::retrieval::XLingRetMethod::computeDocRep(DOCID_T docID) {
  return( new XLingDocModel(docID, &ind, beta, numTarget, 
                            docBasedTargetSmooth));
}

void lemur::retrieval::XLingRetMethod::scoreInvertedIndex(const QueryRep &qRep, 
                                                          IndexedRealVector &scores, 
                                                          bool scoreAll) {

  // for each e in Q
  //   for each a in translates(e)
  //      for each D_a
  //         compute lambda P(a|D)P(e|a)
  int i;
  COUNT_T dc = ind.docCount();
  double s;  
  termScores->reset();
  scAcc.reset();
  const XLingQueryModel *textQR = dynamic_cast<const XLingQueryModel *>(&qRep);
  textQR->startIteration();
  while (textQR->hasMore()) {
    // have to handle oov correctly.
    const XLQueryTerm &qt = textQR->nextTerm();
    double sourceWeight = qt.weight();
    // P(e|GE)
    double pge = qt.getP_s_GE();
    lemur::dictionary::DictEntryVector *xlates = qt.getTranslations();
    if (xlates != NULL) {
      for (lemur::dictionary::DictEntryVector::iterator it = xlates->begin(); 
           it != xlates->end(); it++) {
        TERMID_T tid = ind.term((*it).target);
        if (tid > 0) {
          // P(e|a)
          double prob = (*it).prob;
          DocInfoList *dList = ind.docInfoList(tid);
          DocumentRep *dRep;
          dList->startIteration();
          while (dList->hasMore()) {
            const DocInfo *info = dList->nextEntry();
            DOCID_T id = info->docID();
            if (cacheDocReps) {
              if (docReps[id] == NULL)
                docReps[id] = computeDocRep(id);      
              dRep = docReps[id];
            } else {
              dRep = computeDocRep(id);
            }
            // P(a|D)P(e|a)
            double myScore = matchedTermWeight(tid, prob, info, dRep);
            termScores->incScore(id, myScore);
            if (! cacheDocReps)
              delete(dRep);
          }
          delete dList;
        }
      }
      delete(xlates);

      // log((lambda * 0) + (1 - lambda)P(e|GE))
      double nullScore = adjustedScore(0, pge);
      double finalScore;
      for (i = 1; i <= dc; i++) {
        if (termScores->findScore(i, s)) {
          // log(lambda * Sum_a(P(a|D)P(e|a)) + (1 - lambda)P(e|GE))
          finalScore = adjustedScore(s, pge);
        } else {
          // log((lambda * 0) + (1 - lambda)P(e|GE))
          finalScore = nullScore;
        }
        // f(e) * log(lambda * Sum_a(P(a|D)P(e|a)) + (1 - lambda)P(e|GE))
        finalScore *= sourceWeight;
        scAcc.incScore(i, finalScore);
      }
      termScores->reset();
    } else {
      cerr << "no translations for " << qt.getSource() << endl;
      // f(e) * log((lambda * 0) + (1 - lambda)P(e|GE))
      double nullScore = adjustedScore(0, pge) * sourceWeight;
      for (i = 1; i <= dc; i++) {
        scAcc.incScore(i, nullScore);
      }
    }
  }

  scores.clear();
  for (i = 1; i <= dc; i++) {
    if (scAcc.findScore(i, s)) {
      scores.PushValue(i, s);
    } else if (scoreAll) {
      scores.PushValue(i, 0);
    }
  }
}

double lemur::retrieval::XLingRetMethod::scoreDoc(const QueryRep &qry, DOCID_T docID) {
  lemur::utility::HashFreqVector docVector(ind,docID);
  return (scoreDocVector(*((const XLingQueryModel *)(&qry)),docID,docVector));
}

double lemur::retrieval::XLingRetMethod::scoreDocVector(const XLingQueryModel &qRep, DOCID_T docID, 
                                                        lemur::utility::FreqVector &docVector) {
  double score = 0, termScore;
  qRep.startIteration();
  DocumentRep *dRep = computeDocRep(docID);
  DocInfo *dInfo;
  while (qRep.hasMore()) {
    const XLQueryTerm &qt = qRep.nextTerm();
    double sourceWeight = qt.weight();
    // P(e|GE)
    double pge = qt.getP_s_GE();
    lemur::dictionary::DictEntryVector *xlates = qt.getTranslations();
    if (xlates != NULL) {
      termScore = 0;
      for (lemur::dictionary::DictEntryVector::iterator it = xlates->begin(); 
           it != xlates->end(); it++) {
        TERMID_T tid = ind.term((*it).target);
        if (tid > 0) {
          double prob = (*it).prob;
          int fq;
          if (docVector.find(tid,fq)) {
            dInfo = new DocInfo(docID, fq);
            // P(a|D)P(e|a)
            termScore += matchedTermWeight(tid, prob, dInfo, dRep);
            delete dInfo;
          }
        }
      }
      delete(xlates);
      // f(e) * log(lambda * Sum_a(P(a|D)P(e|a)) + (1 - lambda)P(e|GE))
      score = sourceWeight * adjustedScore(termScore, pge);
    }
  }
  delete(dRep);
  return score;
}
