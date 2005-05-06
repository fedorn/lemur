/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

/** czhai */

#include "TextQueryRetMethod.hpp"
#include "RetParamManager.hpp"
#include "DocInfoList.hpp"

lemur::api::TextQueryRetMethod::TextQueryRetMethod(const Index &ind, 
                                                   ScoreAccumulator & accumulator)  : 
  RetrievalMethod(ind), scAcc(accumulator) {
  // have to fix this to be passed in.
  RetrievalParameter::get();
  cacheDocReps = RetrievalParameter::cacheDocReps;
  
  if (cacheDocReps) {
    COUNT_T dc = ind.docCount();
    docRepsSize = dc + 1;
    docReps = new DocumentRep *[docRepsSize];
    for (COUNT_T i = 0; i <= dc; i++) docReps[i] = NULL;
  }
}

void lemur::api::TextQueryRetMethod::scoreCollection(DOCID_T docID, 
                                                     IndexedRealVector &results){
  QueryRep *rep = computeTextQueryRep(docID);
  scoreCollection(*rep, results);
  delete(rep);
}
//fix this to pass scoreAll down.
void lemur::api::TextQueryRetMethod::scoreCollection(const QueryRep &qry, 
                                                     IndexedRealVector &results) {
  scoreInvertedIndex(qry, results);
}

void lemur::api::TextQueryRetMethod::scoreInvertedIndex(const QueryRep &qRep, 
                                                        IndexedRealVector &scores, 
                                                        bool scoreAll) {

  scAcc.reset();
  int i;
  TextQueryRep *textQR = (TextQueryRep *)(&qRep);
  textQR->startIteration();
  
  while (textQR->hasMore()) {
    QueryTerm *qTerm = textQR->nextTerm();
    DocInfoList *dList = ind.docInfoList(qTerm->id());
    DocumentRep *dRep;
    dList->startIteration();
    while (dList->hasMore()) {
      DocInfo *info = dList->nextEntry();
      DOCID_T id = info->docID();
      if (cacheDocReps) {
        if (docReps[id] == NULL)
          docReps[id] = computeDocRep(id);      
        dRep = docReps[id];
      } else {
        dRep = computeDocRep(id);
      }
      scAcc.incScore(id, scoreFunc()->matchedTermWeight(qTerm, textQR, info, 
                                                        dRep));
      if (! cacheDocReps)
        delete(dRep);
    }
    delete dList;
    delete qTerm;
  }

  scores.clear();
  double s;
  DocumentRep *dr;
  for (i=1; i<=ind.docCount(); i++) {
    dr = NULL;
    if (scAcc.findScore(i,s)) {
      if (cacheDocReps)
        dr = docReps[i]; // must be there if there is a score.
      else
        dr = computeDocRep(i);
      scores.PushValue(i, scoreFunc()->adjustedScore(s, textQR, dr));
      if (! cacheDocReps)
        delete(dr);
    } else if (scoreAll) {
      if (cacheDocReps) {
        if (docReps[i] == NULL)
          docReps[i] = computeDocRep(i);
        dr = docReps[i]; // must be there.
      } else {
        dr = computeDocRep(i);
      }
      scores.PushValue(i, scoreFunc()->adjustedScore(0, textQR, dr));
      if (! cacheDocReps)
        delete(dr);
    }
  }
}

double lemur::api::TextQueryRetMethod::scoreDoc(const QueryRep &qry, DOCID_T docID) {
  lemur::utility::HashFreqVector docVector(ind,docID);
  return (scoreDocVector(*((TextQueryRep *)(&qry)),docID,docVector));
}

double lemur::api::TextQueryRetMethod::scoreDocVector(const TextQueryRep &qRep, DOCID_T docID, 
                                                      lemur::utility::FreqVector &docVector) {

  qRep.startIteration();
  
  double score=0;

  DocumentRep *dRep = computeDocRep(docID);

  DocInfo *dInfo;
  while (qRep.hasMore()) {
    QueryTerm *qTerm = qRep.nextTerm();
    int fq;
    
    if (docVector.find(qTerm->id(),fq)) {
      dInfo = new DocInfo(docID, fq);
      score += scoreFunc()
        ->matchedTermWeight(qTerm, &qRep, dInfo, dRep);
      delete dInfo;
    }
    delete qTerm;
  }
  score = scoreFunc()->adjustedScore(score, &qRep,  dRep);
  delete dRep;
  return score;
}

#include <float.h> // for -DBL_MAX
double lemur::api::TextQueryRetMethod::scoreDocPassages(const TermQuery &qry, DOCID_T docID, 
                                                        lemur::retrieval::PassageScoreVector &scores, 
                                                        int psgSize, int overlap) {
  double score = 0, maxScore = -DBL_MAX;
  MatchInfo *matches = MatchInfo::getMatches(ind, qry, docID);
  TextQueryRep *qRep = (TextQueryRep *) computeQueryRep(qry);
  DocumentRep *dRep = computeDocRep(docID);
  lemur::retrieval::PassageRep *pRep, *myRep;
  // passage start + length needed for ret (alternate vector class).
  int pNum = 1;
  scores.clear();
  myRep = new lemur::retrieval::PassageRep(*dRep, ind.docLength(docID), psgSize, overlap);
  
  // passage info here
  DocInfo *dInfo;
  //  pRep->startPassageIteration();
  //  while (pRep->hasMorePassage()) {
  for (lemur::retrieval::PassageRep::iterator iter = myRep->begin(), endIter = myRep->end();
       iter != endIter;
       iter++) {    
    score = 0;
    pRep = &(*iter);
    qRep->startIteration();
    while (qRep->hasMore()) {
      QueryTerm *qTerm = qRep->nextTerm();
      COUNT_T fq = pRep->passageTF(qTerm->id(), matches);
      dInfo = new DocInfo(docID, fq); //passage freq here
      score += scoreFunc()->matchedTermWeight(qTerm, qRep, dInfo, pRep);
      delete dInfo;
      delete qTerm;
    }
    score = scoreFunc()->adjustedScore(score, qRep,  pRep);
    lemur::retrieval::PassageScore s;
    s.id = pNum++;
    s.start = pRep->getStart();
    s.end = pRep->getEnd();
    s.score = score;
    
    scores.push_back(s);
    if (score > maxScore) {
      maxScore = score;
    }
    //    pRep->nextPassage();
  }
  delete(matches);
  delete(dRep);
  //  delete(pRep);  
  delete(myRep);
  delete(qRep);
  return maxScore;
}
