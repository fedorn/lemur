/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */



#include "SimpleKLRetMethod.hpp"
#include "Param.hpp"
#include "common_headers.hpp"
#include <cmath>
#include "DocUnigramCounter.hpp"
#include "RelDocUnigramCounter.hpp"
#include "OneStepMarkovChain.hpp"

using namespace lemur::api;

void lemur::retrieval::SimpleKLQueryModel::interpolateWith(const lemur::langmod::UnigramLM &qModel, 
                                                           double origModCoeff, 
                                                           int howManyWord, 
                                                           double prSumThresh, 
                                                           double prThresh) {
  if (!qm) {
    qm = new lemur::api::IndexedRealVector();
  } else {
    qm->clear();
  }

  qModel.startIteration();
  while (qModel.hasMore()) {
    IndexedReal entry;
    qModel.nextWordProb((TERMID_T &)entry.ind,entry.val);
    qm->push_back(entry);

  }
  qm->Sort();
  
  double countSum = totalCount();

  // discounting the original model
  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight()*origModCoeff/countSum);
    delete qt;
  }
  
  // now adding the new model
  double prSum = 0;
  int wdCount = 0;
  IndexedRealVector::iterator it;
  it = qm->begin();
  while (it != qm->end() && prSum < prSumThresh && 
         wdCount < howManyWord && (*it).val >=prThresh) {
    incCount((*it).ind, (*it).val*(1-origModCoeff));
    prSum += (*it).val;
    it++;
    wdCount++;
  }

  //Sum w in Q qtf * log(qtcf/termcount);
  colQLikelihood = 0;
  colQueryLikelihood();
  colKLComputed = false;
}

void lemur::retrieval::SimpleKLQueryModel::load(istream &is)
{
  // clear existing counts
  startIteration();
  QueryTerm *qt;
  while (hasMore()) {
    qt = nextTerm();
    setCount(qt->id(),0);
  }
  colQLikelihood = 0;

  int count;
  is >> count;
  char wd[500];
  double pr;
  while (count-- >0) {
    is >> wd >> pr;
    TERMID_T id = ind.term(wd);
    if (id != 0) setCount(id, pr); // don't load OOV terms
  }
  colQueryLikelihood();
  colKLComputed = false;
}

void lemur::retrieval::SimpleKLQueryModel::save(ostream &os)
{
  int count = 0;
  startIteration();
  QueryTerm *qt;
  while (hasMore()) {
    qt = nextTerm();
    count++;
    delete qt;
  }
  os << " " << count << endl;
  startIteration();
  while (hasMore()) {
    qt = nextTerm();
    os << ind.term(qt->id()) << " "<< qt->weight() << endl;
    delete qt;
  }
}

void lemur::retrieval::SimpleKLQueryModel::clarity(ostream &os)
{
  int count = 0;
  double sum=0, ln_Pr=0;
  startIteration();
  QueryTerm *qt;
  while (hasMore()) {
    qt = nextTerm();
    count++;
    // query-clarity = SUM_w{P(w|Q)*log(P(w|Q)/P(w))}
    // P(w)=cf(w)/|C|
    double pw = ((double)ind.termCount(qt->id())/(double)ind.termCount());
    // P(w|Q) is a prob computed by any model, e.g. relevance models
    double pwq = qt->weight();
    sum += pwq;    
    ln_Pr += (pwq)*log(pwq/pw);
    delete qt;
  }
  // clarity should be computed with log_2, so divide by log(2).
  os << "=" << count << " " << (ln_Pr/(sum ? sum : 1.0)/log(2.0)) << endl;
  startIteration();
  while (hasMore()) {
    qt = nextTerm();
    // print clarity for each query term
    // clarity should be computed with log_2, so divide by log(2).
    os << ind.term(qt->id()) << " "
       << (qt->weight()*log(qt->weight()/
                            ((double)ind.termCount(qt->id())/
                             (double)ind.termCount())))/log(2.0) << endl;
    delete qt;
  }
}

double lemur::retrieval::SimpleKLQueryModel::clarity() const
{
  int count = 0;
  double sum=0, ln_Pr=0;
  startIteration();
  QueryTerm *qt;
  while (hasMore()) {
    qt = nextTerm();
    count++;
    // query-clarity = SUM_w{P(w|Q)*log(P(w|Q)/P(w))}
    // P(w)=cf(w)/|C|
    double pw = ((double)ind.termCount(qt->id())/(double)ind.termCount());
    // P(w|Q) is a prob computed by any model, e.g. relevance models
    double pwq = qt->weight();
    sum += pwq;    
    ln_Pr += (pwq)*log(pwq/pw);
    delete qt;
  }
  // normalize by sum of probabilities in the input model
  ln_Pr = ln_Pr/(sum ? sum : 1.0);
  // clarity should be computed with log_2, so divide by log(2).
  return (ln_Pr/log(2.0));
}

lemur::retrieval::SimpleKLRetMethod::SimpleKLRetMethod(const Index &dbIndex, 
                                                       const string &supportFileName, 
                                                       ScoreAccumulator &accumulator) : 
  TextQueryRetMethod(dbIndex, accumulator), supportFile(supportFileName) {

  docParam.smthMethod = SimpleKLParameter::defaultSmoothMethod;
  docParam.smthStrategy= SimpleKLParameter::defaultSmoothStrategy;
  docParam.ADDelta = SimpleKLParameter::defaultADDelta;
  docParam.JMLambda = SimpleKLParameter::defaultJMLambda;
  docParam.DirPrior = SimpleKLParameter::defaultDirPrior;

  qryParam.adjScoreMethod = SimpleKLParameter::NEGATIVEKLD;
  qryParam.fbMethod = SimpleKLParameter::defaultFBMethod;
  qryParam.fbCoeff = SimpleKLParameter::defaultFBCoeff;
  qryParam.fbPrTh = SimpleKLParameter::defaultFBPrTh;
  qryParam.fbPrSumTh = SimpleKLParameter::defaultFBPrSumTh;
  qryParam.fbTermCount = SimpleKLParameter::defaultFBTermCount;
  qryParam.fbMixtureNoise = SimpleKLParameter::defaultFBMixNoise;
  qryParam.emIterations = SimpleKLParameter::defaultEMIterations;

  docProbMass = NULL;
  uniqueTermCount = NULL;
  mcNorm = NULL;

  collectLMCounter = new lemur::langmod::DocUnigramCounter(ind);
  collectLM = new lemur::langmod::MLUnigramLM(*collectLMCounter, ind.termLexiconID()); 

  scFunc = new SimpleKLScoreFunc();
  scFunc->setScoreMethod(qryParam.adjScoreMethod);
}

lemur::retrieval::SimpleKLRetMethod::~SimpleKLRetMethod() 
{
  delete [] docProbMass;
  delete [] uniqueTermCount;
  delete [] mcNorm;
  delete collectLM;
  delete collectLMCounter;
  delete scFunc;
}

void lemur::retrieval::SimpleKLRetMethod::loadSupportFile() {
  ifstream ifs;
  int i;

  // Only need to load this file if smooth strategy is back off
  // or the smooth method is absolute discount. Don't reload if
  // docProbMass is not NULL.

  if (docProbMass == NULL &&
      (docParam.smthMethod == SimpleKLParameter::ABSOLUTEDISCOUNT ||
       docParam.smthStrategy == SimpleKLParameter::BACKOFF)) {
    cerr << "lemur::retrieval::SimpleKLRetMethod::loadSupportFile loading "
         << supportFile << endl;
      
    ifs.open(supportFile.c_str());
    if (ifs.fail()) {
      throw  Exception("lemur::retrieval::SimpleKLRetMethod::loadSupportFile", 
                       "smoothing support file open failure");
    }
    COUNT_T numDocs = ind.docCount();
    docProbMass = new double[numDocs+1];
    uniqueTermCount = new COUNT_T[numDocs+1];
    for (i = 1; i <= numDocs; i++) {
      DOCID_T id;
      int uniqCount;
      double prMass;
      ifs >> id >> uniqCount >> prMass;
      if (id != i) {
        throw  Exception("lemur::retrieval::SimpleKLRetMethod::loadSupportFile", 
                         "alignment error in smooth support file, wrong id:");
      }
      docProbMass[i] = prMass;
      uniqueTermCount[i] = uniqCount;
    }
    ifs.close();
  }
    
  // only need to load this file if the feedback method is
  // markov chain. Don't reload if called a second time.

  if (mcNorm == NULL && qryParam.fbMethod == SimpleKLParameter::MARKOVCHAIN) {
    string mcSuppFN = supportFile + ".mc";
    cerr << "lemur::retrieval::SimpleKLRetMethod::loadSupportFile loading " << mcSuppFN << endl;

    ifs.open(mcSuppFN.c_str());
    if (ifs.fail()) {
      throw Exception("lemur::retrieval::SimpleKLRetMethod::loadSupportFile", 
                      "Markov chain support file can't be opened");
    }

    mcNorm = new double[ind.termCountUnique()+1];
  
    for (i = 1; i <= ind.termCountUnique(); i++) {
      TERMID_T id;
      double norm;
      ifs >> id >> norm;
      if (id != i) {
        throw Exception("lemur::retrieval::SimpleKLRetMethod::loadSupportFile", 
                        "alignment error in Markov chain support file, wrong id:");
      }
      mcNorm[i] = norm;
    }
  }
}

DocumentRep *lemur::retrieval::SimpleKLRetMethod::computeDocRep(DOCID_T docID)
{
  switch (docParam.smthMethod) {
  case SimpleKLParameter::JELINEKMERCER:
    return( new JelinekMercerDocModel(docID,
                                      ind.docLength(docID),
                                      *collectLM,
                                      docProbMass,
                                      docParam.JMLambda,
                                      docParam.smthStrategy));
  case SimpleKLParameter::DIRICHLETPRIOR:
    return (new DirichletPriorDocModel(docID,
                                       ind.docLength(docID),
                                       *collectLM,
                                       docProbMass,  
                                       docParam.DirPrior,
                                       docParam.smthStrategy));
  case SimpleKLParameter::ABSOLUTEDISCOUNT:
    return (new AbsoluteDiscountDocModel(docID,
                                         ind.docLength(docID),
                                         *collectLM,
                                         docProbMass,
                                         uniqueTermCount,
                                         docParam.ADDelta,
                                         docParam.smthStrategy));
  case SimpleKLParameter::TWOSTAGE:
    return (new TwoStageDocModel(docID,
                                 ind.docLength(docID),
                                 *collectLM,
                                 docProbMass,
                                 docParam.DirPrior, // 1st stage mu
                                 docParam.JMLambda, // 2nd stage lambda
                                 docParam.smthStrategy));
    
    
  default:
    // this should throw, not exit.
    cerr << "Unknown document language model smoothing method\n";
    exit(1);
  }
}


void lemur::retrieval::SimpleKLRetMethod::updateTextQuery(TextQueryRep &origRep, 
                                                          const DocIDSet &relDocs)
{
  SimpleKLQueryModel *qr;

  qr = dynamic_cast<SimpleKLQueryModel *> (&origRep);
 
  switch (qryParam.fbMethod) {
  case SimpleKLParameter::MIXTURE:
    computeMixtureFBModel(*qr, relDocs);
    break;
  case SimpleKLParameter::DIVMIN:
    computeDivMinFBModel(*qr, relDocs);
    break;
  case SimpleKLParameter::MARKOVCHAIN:
    computeMarkovChainFBModel(*qr, relDocs);
    break;
  case SimpleKLParameter::RM1:
    computeRM1FBModel(*qr, relDocs);
    break;
  case SimpleKLParameter::RM2:
    computeRM2FBModel(*qr, relDocs);
    break;   
  default:
    throw Exception("SimpleKLRetMethod", "unknown feedback method");
    break;
  }
}


void lemur::retrieval::SimpleKLRetMethod::computeMixtureFBModel(SimpleKLQueryModel &origRep, 
                                                                const DocIDSet &relDocs)
{
  COUNT_T numTerms = ind.termCountUnique();

  lemur::langmod::DocUnigramCounter *dCounter = new lemur::langmod::DocUnigramCounter(relDocs, ind);

  double *distQuery = new double[numTerms+1];
  double *distQueryEst = new double[numTerms+1];

  double noisePr;

  int i;

  double meanLL=1e-40;
  double distQueryNorm=0;

  for (i=1; i<=numTerms;i++) {
    distQueryEst[i] = rand()+0.001;
    distQueryNorm+= distQueryEst[i];
  }
  noisePr = qryParam.fbMixtureNoise;

  int itNum = qryParam.emIterations;
  do {
    // re-estimate & compute likelihood
    double ll = 0;

    for (i=1; i<=numTerms;i++) {
        
      distQuery[i] = distQueryEst[i]/distQueryNorm;
      // cerr << "dist: "<< distQuery[i] << endl;
      distQueryEst[i] =0;
    }

    distQueryNorm = 0;

    // compute likelihood
    dCounter->startIteration();
    while (dCounter->hasMore()) {
      int wd; //dmf FIXME
      double wdCt;
      dCounter->nextCount(wd, wdCt);
      ll += wdCt * log (noisePr*collectLM->prob(wd)  // Pc(w)
                        + (1-noisePr)*distQuery[wd]); // Pq(w)
    }
    meanLL = 0.5*meanLL + 0.5*ll;
    if (fabs((meanLL-ll)/meanLL)< 0.0001) {
      cerr << "converged at "<< qryParam.emIterations - itNum+1 
           << " with likelihood= "<< ll << endl;
      break;
    } 

    // update counts

    dCounter->startIteration();
    while (dCounter->hasMore()) {
      int wd; // dmf FIXME
      double wdCt;
      dCounter->nextCount(wd, wdCt);
      
      double prTopic = (1-noisePr)*distQuery[wd]/
        ((1-noisePr)*distQuery[wd]+noisePr*collectLM->prob(wd));

      double incVal = wdCt*prTopic;
      distQueryEst[wd] += incVal;
      distQueryNorm += incVal;
    }
  } while (itNum-- > 0);
  
  lemur::utility::ArrayCounter<double> lmCounter(numTerms+1);
  for (i=1; i<=numTerms; i++) {
    if (distQuery[i] > 0) {
      lmCounter.incCount(i, distQuery[i]);
    }
  }
  lemur::langmod::MLUnigramLM *fblm = new lemur::langmod::MLUnigramLM(lmCounter, ind.termLexiconID());
  origRep.interpolateWith(*fblm, (1-qryParam.fbCoeff), qryParam.fbTermCount,
                          qryParam.fbPrSumTh, qryParam.fbPrTh);
  delete fblm;
  delete dCounter;
  delete[] distQuery;
  delete[] distQueryEst;
}


void lemur::retrieval::SimpleKLRetMethod::computeDivMinFBModel(SimpleKLQueryModel &origRep, 
                                                               const DocIDSet &relDocs)
{
  COUNT_T numTerms = ind.termCountUnique();

  double * ct = new double[numTerms+1];

  TERMID_T i;
  for (i=1; i<=numTerms; i++) ct[i]=0;

  COUNT_T actualDocCount=0;
  relDocs.startIteration();
  while (relDocs.hasMore()) {
    actualDocCount++;
    int id;
    double pr;
    relDocs.nextIDInfo(id,pr);
    SimpleKLDocModel *dm;
    dm = dynamic_cast<SimpleKLDocModel *> (computeDocRep(id));

    for (i=1; i<=numTerms; i++) { // pretend every word is unseen
      ct[i] += log(dm->unseenCoeff()*collectLM->prob(i));
    }

    TermInfoList *tList = ind.termInfoList(id);
    TermInfo *info;
    tList->startIteration();
    while (tList->hasMore()) {
      info = tList->nextEntry();
      ct[info->termID()] += log(dm->seenProb(info->count(), info->termID())/
                                (dm->unseenCoeff()*collectLM->prob(info->termID())));
    }
    delete tList;
    delete dm;
  }
  if (actualDocCount==0) return;

  lemur::utility::ArrayCounter<double> lmCounter(numTerms+1);
  
  double norm = 1.0/(double)actualDocCount;
  for (i=1; i<=numTerms; i++) { 
    lmCounter.incCount(i, 
                       exp((ct[i]*norm -
                            qryParam.fbMixtureNoise*log(collectLM->prob(i)))
                           / (1.0-qryParam.fbMixtureNoise)));
  }
  delete [] ct;

  
  lemur::langmod::MLUnigramLM *fblm = new lemur::langmod::MLUnigramLM(lmCounter, ind.termLexiconID());
  origRep.interpolateWith(*fblm, (1-qryParam.fbCoeff), qryParam.fbTermCount,
                          qryParam.fbPrSumTh, qryParam.fbPrTh);
  delete fblm;
}

void lemur::retrieval::SimpleKLRetMethod::computeMarkovChainFBModel(SimpleKLQueryModel &origRep, const DocIDSet &relDocs)
{
  int stopWordCutoff =50;

  lemur::utility::ArrayCounter<double> *counter = new lemur::utility::ArrayCounter<double>(ind.termCountUnique()+1);

  lemur::langmod::OneStepMarkovChain * mc = new lemur::langmod::OneStepMarkovChain(relDocs, ind, mcNorm,
                                                                                   1-qryParam.fbMixtureNoise);
  origRep.startIteration();
  double summ;
  while (origRep.hasMore()) {
    QueryTerm *qt;
    qt = origRep.nextTerm();
    summ =0;
    mc->startFromWordIteration(qt->id());
    // cout << " +++++++++ "<< ind.term(qt->id()) <<endl;
    TERMID_T fromWd;
    double fromWdPr;
    
    while (mc->hasMoreFromWord()) {
      mc->nextFromWordProb(fromWd, fromWdPr);
      if (fromWd <= stopWordCutoff) { // a stop word
        continue;
      }
      summ += qt->weight()*fromWdPr*collectLM->prob(fromWd);
      // summ += qt->weight()*fromWdPr;
    }
    if (summ==0) {
      // query term doesn't exist in the feedback documents, skip
      continue;
    }

    mc->startFromWordIteration(qt->id());
    while (mc->hasMoreFromWord()) {
      mc->nextFromWordProb(fromWd, fromWdPr);
      if (fromWd <= stopWordCutoff) { // a stop word
        continue;
      }

      counter->incCount(fromWd, 
                        (qt->weight()*fromWdPr*collectLM->prob(fromWd)/summ));
      // counter->incCount(fromWd, (qt->weight()*fromWdPr/summ));

    }
    delete qt;
  }
  delete mc;

  lemur::langmod::UnigramLM *fbLM = new lemur::langmod::MLUnigramLM(*counter, ind.termLexiconID());

  origRep.interpolateWith(*fbLM, 1-qryParam.fbCoeff, qryParam.fbTermCount,
                          qryParam.fbPrSumTh, qryParam.fbPrTh);
  delete fbLM;
  delete counter;
}

void lemur::retrieval::SimpleKLRetMethod::computeRM1FBModel(SimpleKLQueryModel &origRep, 
                                                            const DocIDSet &relDocs)
{  
  COUNT_T numTerms = ind.termCountUnique();

  // RelDocUnigramCounter computes SUM(D){P(w|D)*P(D|Q)} for each w
  lemur::langmod::RelDocUnigramCounter *dCounter = new lemur::langmod::RelDocUnigramCounter(relDocs, ind);

  double *distQuery = new double[numTerms+1];
  double expWeight = qryParam.fbCoeff;

  TERMID_T i;
  for (i=1; i<=numTerms;i++)
    distQuery[i] = 0.0;

  double pSum=0.0;
  dCounter->startIteration();
  while (dCounter->hasMore()) {
    int wd; // dmf FIXME
    double wdPr;
    dCounter->nextCount(wd, wdPr);
    distQuery[wd]=wdPr;
    pSum += wdPr;
  }

  for (i=1; i<=numTerms;i++) {
    distQuery[i] = expWeight*distQuery[i]/pSum +
      (1-expWeight)*ind.termCount(i)/ind.termCount();
  }

  lemur::utility::ArrayCounter<double> lmCounter(numTerms+1);
  for (i=1; i<=numTerms; i++) {
    if (distQuery[i] > 0) {
      lmCounter.incCount(i, distQuery[i]);
    }
  }
  lemur::langmod::MLUnigramLM *fblm = new lemur::langmod::MLUnigramLM(lmCounter, ind.termLexiconID());
  origRep.interpolateWith(*fblm, 0.0, qryParam.fbTermCount,
                          qryParam.fbPrSumTh, 0.0);
  delete fblm;
  delete dCounter;
  delete[] distQuery;
}

// out: w.weight = P(w|Q)
// P(w|Q) = k P(w) P(Q|w)
// P(Q|w) = PROD_q P(q|w)
// P(q|w) = SUM_d P(q|d) P(w|d) p(d) / p(w)
// P(w) = SUM_d P(w|d) p(d)
// Promote this to some include somewhere...
struct termProb  {
  TERMID_T id; // TERM_ID
  double prob; // a*tf(w,d)/|d| +(1-a)*tf(w,C)/|C|
};

void lemur::retrieval::SimpleKLRetMethod::computeRM2FBModel(SimpleKLQueryModel &origRep, 
                                                            const DocIDSet &relDocs) {  
  COUNT_T numTerms = ind.termCountUnique();
  COUNT_T termCount = ind.termCount();
  double expWeight = qryParam.fbCoeff;

  // RelDocUnigramCounter computes P(w)=SUM(D){P(w|D)*P(D|Q)} for each w
  // P(w) = SUM_d P(w|d) p(d)
  lemur::langmod::RelDocUnigramCounter *dCounter = new lemur::langmod::RelDocUnigramCounter(relDocs, ind);

  double *distQuery = new double[numTerms+1];
  COUNT_T numDocs = ind.docCount();
  vector<termProb> **tProbs = new vector<termProb> *[numDocs + 1];

  int i;
  for (i=1; i<=numTerms;i++) 
    distQuery[i] = 0.0;
  for (i = 1; i <= numDocs; i++) {
    tProbs[i] = NULL;
  }
  
  // Put these in a faster structure.
  vector <TERMID_T> qTerms; // TERM_ID
  origRep.startIteration();
  while (origRep.hasMore()) {
    QueryTerm *qt = origRep.nextTerm();
    qTerms.push_back(qt->id());
    delete(qt);
  }
  COUNT_T numQTerms = qTerms.size();
  dCounter->startIteration();
  while (dCounter->hasMore()) {
    int wd; // dmf fixme
    double P_w;
    double P_qw=0;
    double P_Q_w = 1.0;
    // P(q|w) = SUM_d P(q|d) P(w|d) p(d)
    dCounter->nextCount(wd, P_w);
    for (int j = 0; j < numQTerms; j++) {
      TERMID_T qtID = qTerms[j]; // TERM_ID
      relDocs.startIteration();
      while (relDocs.hasMore()) {
        int docID;
        double P_d, P_w_d, P_q_d;
        double dlength;
        relDocs.nextIDInfo(docID, P_d);
        dlength  = (double)ind.docLength(docID);
        if (tProbs[docID] == NULL) {
          vector<termProb> * pList = new vector<termProb>;
          TermInfoList *tList = ind.termInfoList(docID);
          TermInfo *t;
          tList->startIteration();
          while (tList->hasMore()) {
            t = tList->nextEntry();
            termProb prob;
            prob.id = t->termID();
            prob.prob = expWeight*t->count()/dlength+
              (1-expWeight)*ind.termCount(t->termID())/termCount;
            pList->push_back(prob);
          }
          delete(tList);
          tProbs[docID] = pList;
        }
        vector<termProb> * pList = tProbs[docID];       
        P_w_d=0;
        P_q_d=0;
        for (int i = 0; i < pList->size(); i++) {         
          // p(q|d)= a*tf(q,d)/|d|+(1-a)*tf(q,C)/|C|
          if((*pList)[i].id == qtID)
            P_q_d = (*pList)[i].prob;     
          // p(w|d)= a*tf(w,d)/|d|+(1-a)*tf(w,C)/|C|
          if((*pList)[i].id == wd)
            P_w_d = (*pList)[i].prob;
          if(P_q_d && P_w_d)
            break;
        }
        P_qw += P_d*P_w_d*P_q_d;
      }
      // P(Q|w) = PROD_q P(q|w) / p(w)
      P_Q_w *= P_qw/P_w;
    }
    // P(w|Q) = k P(w) P(Q|w), k=1
    distQuery[wd] =P_w*P_Q_w;
  }

  lemur::utility::ArrayCounter<double> lmCounter(numTerms+1);
  for (i=1; i<=numTerms; i++) {
    if (distQuery[i] > 0) {
      lmCounter.incCount(i, distQuery[i]);
    }
  }
  lemur::langmod::MLUnigramLM *fblm = new lemur::langmod::MLUnigramLM(lmCounter, ind.termLexiconID());
  origRep.interpolateWith(*fblm, 0.0, qryParam.fbTermCount,
                          qryParam.fbPrSumTh, 0.0);
  delete fblm;
  delete dCounter;
  for (i = 1; i <= numDocs; i++) {
    delete(tProbs[i]);
  }
  delete[](tProbs);
  delete[] distQuery;
}



