/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "SimpleKLRetMethod.hpp"
#include "Param.hpp"
#include "common_headers.hpp"
#include <cmath>
#include "DocUnigramCounter.hpp"
#include "OneStepMarkovChain.hpp"

void SimpleKLQueryModel::interpolateWith(UnigramLM &qModel, double origModCoeff, int howManyWord, double prSumThresh, double prThresh) 
{
  if (!qm) {
    qm = new IndexedRealVector();
  } else {
    qm->clear();
  }

  qModel.startIteration();
  while (qModel.hasMore()) {
    IndexedReal entry;
    qModel.nextWordProb(entry.ind,entry.val);
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
  while (it != qm->end() && prSum < prSumThresh && wdCount < howManyWord && (*it).val >=prThresh) {
    // cout << "==new-term== " << ind.term((*it).ind) << " "<< (*it).val << endl;
    incCount((*it).ind, (*it).val*(1-origModCoeff));
    it++;
    prSum += (*it).val;
    wdCount++;
  }
}

void SimpleKLQueryModel::load(istream &is)
{
  // clear existing counts
  startIteration();
  QueryTerm *qt;
  while (hasMore()) {
    qt = nextTerm();
    setCount(qt->id(),0);
  }
  int count;
  is >> count;
  char wd[500];
  double pr;
  while (count-- >0) {
    is >> wd >> pr;
    setCount(ind.term(wd), pr);
  }
}

void SimpleKLQueryModel::save(ostream &os)
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


SimpleKLRetMethod::SimpleKLRetMethod(Index &dbIndex, const char *supportFileName, ScoreAccumulator &accumulator) : 
  TextQueryRetMethod(dbIndex, accumulator)
{

  docParam.smthMethod = SimpleKLParameter::defaultSmoothMethod;
  docParam.smthStrategy= SimpleKLParameter::defaultSmoothStrategy;
  docParam.ADDelta = SimpleKLParameter::defaultADDelta;
  docParam.JMLambda = SimpleKLParameter::defaultJMLambda;
  docParam.DirPrior = SimpleKLParameter::defaultDirPrior;


  qryParam.fbMethod = SimpleKLParameter::defaultFBMethod;
  qryParam.fbCoeff = SimpleKLParameter::defaultFBCoeff;
  qryParam.fbPrTh = SimpleKLParameter::defaultFBPrTh;
  qryParam.fbPrSumTh = SimpleKLParameter::defaultFBPrSumTh;
  qryParam.fbTermCount = SimpleKLParameter::defaultFBTermCount;
  qryParam.fbMixtureNoise = SimpleKLParameter::defaultFBMixNoise;
  qryParam.emIterations = SimpleKLParameter::defaultEMIterations;

  ifstream ifs;
  ifs.open(supportFileName);
  if (ifs.fail()) {
    throw  Exception("SimpleKLRetMethod", "smoothing support file open failure");
  }
  int numDocs = ind.docCount();
  docProbMass = new double[numDocs+1];
  uniqueTermCount = new int[numDocs+1];
  
  int i;
  
  for (i=1; i<= numDocs; i++) {
    int id, uniqCount;
    double prMass;
    ifs >> id >> uniqCount >> prMass;
    if (id != i) {
      cerr << "alignment error in smoothing support file, wrong id:" << id << endl; 
      exit(1);
    }
    docProbMass[i]=prMass;
    uniqueTermCount[i]=uniqCount;
  }
  
  ifs.close();

  collectLMCounter = new DocUnigramCounter(ind);
  //  collectLM = new MLUnigramLM(*collectLMCounter, ind.termLexiconID()); 
  collectLM = new LaplaceUnigramLM(*collectLMCounter, ind.termLexiconID(), 
				   ind.termCountUnique()); 


  char mcSuppFN[500];
  strcpy(mcSuppFN, supportFileName);
  strcat(mcSuppFN, ".mc");
  ifs.open(mcSuppFN);
  if (ifs.fail()) {
    throw Exception("SimpleKLRetMethod", "Markov chain support file can't be opened");
  }

  mcNorm = new double[ind.termCountUnique()+1];
  
  for (i=1; i<= ind.termCountUnique(); i++) {
    int id;
    double norm;
    ifs >> id >> norm;
    if (id != i) {
      cerr << "alignment error in Markov chain support file, wrong id:" << id << endl; 
      exit(1);
    }
    mcNorm[i] = norm;
  }

  scFunc = new SimpleKLScoreFunc();
}

SimpleKLRetMethod::~SimpleKLRetMethod() 
{
  delete [] docProbMass;
  delete [] uniqueTermCount;
  delete [] mcNorm;
  delete collectLM;
  delete collectLMCounter;
  delete scFunc;
}

DocumentRep *SimpleKLRetMethod::computeDocRep(int docID)
{
  switch (docParam.smthMethod) {
  case SimpleKLParameter::JELINEKMERCER:
    return( new JelinekMercerDocModel(docID,
				      &ind, 
				      *collectLM,
				      docProbMass,
				      docParam.JMLambda,
				      docParam.smthStrategy));
  case SimpleKLParameter::DIRICHLETPRIOR:
    return (new DirichletPriorDocModel(docID,
				 &ind, 
				 *collectLM,
				 docProbMass,  
				 docParam.DirPrior,
				 docParam.smthStrategy));
  case SimpleKLParameter::ABSOLUTEDISCOUNT:
    return (new AbsoluteDiscountDocModel(docID,
					 &ind, 
					 *collectLM,
					 docProbMass,
					 uniqueTermCount,
					 docParam.ADDelta,
					 docParam.smthStrategy));
  case SimpleKLParameter::TWOSTAGE:
    return (new TwoStageDocModel(docID,
				 &ind, 
				 *collectLM,
				 docProbMass,
				 docParam.DirPrior, // 1st stage mu
				 docParam.JMLambda, // 2nd stage lambda
				 docParam.smthStrategy));
    
    
  default:
    cerr << "Unknown document language model smoothing method\n";
    exit(1);
  }

}


void SimpleKLRetMethod::updateTextQuery(TextQueryRep &origRep, DocIDSet &relDocs)
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
  default:
    throw Exception("SimpleKLRetMethod", "unknown feedback method");
    break;
  }
}


void SimpleKLRetMethod::computeMixtureFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs)
{
  int numTerms = ind.termCountUnique();

  DocUnigramCounter *dCounter = new DocUnigramCounter(relDocs, ind);

  static double *distQuery = new double[numTerms+1];
  static double *distQueryEst = new double[numTerms+1];

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
      int wd;
      double wdCt;
      dCounter->nextCount(wd, wdCt);
      ll += wdCt * log (noisePr*collectLM->prob(wd)  // Pc(w)
			+ (1-noisePr)*distQuery[wd]); // Pq(w)
    }
    meanLL = 0.5*meanLL + 0.5*ll;
    if (fabs((meanLL-ll)/meanLL)< 0.0001) {
      cerr << "converged at "<< qryParam.emIterations - itNum+1 << " with likelihood= "<< ll << endl;
      break;
    } 

    // update counts

    dCounter->startIteration();
    while (dCounter->hasMore()) {
      int wd;
      double wdCt;
      dCounter->nextCount(wd, wdCt);
      
      double prTopic = (1-noisePr)*distQuery[wd]/
	((1-noisePr)*distQuery[wd]+noisePr*collectLM->prob(wd));

      double incVal = wdCt*prTopic;
      distQueryEst[wd] += incVal;
      distQueryNorm += incVal;
    }
  } while (itNum-- > 0);
  
  ArrayCounter<double> lmCounter(numTerms+1);
  for (i=1; i<=numTerms; i++) {
    if (distQuery[i] > 0) {
      lmCounter.incCount(i, distQuery[i]);
    }
  }
  MLUnigramLM *fblm = new MLUnigramLM(lmCounter, ind.termLexiconID());
  origRep.interpolateWith(*fblm, (1-qryParam.fbCoeff), qryParam.fbTermCount,
			qryParam.fbPrSumTh, qryParam.fbPrTh);
  delete fblm;
  delete dCounter;
}


void SimpleKLRetMethod::computeDivMinFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs)
{
  int numTerms = ind.termCountUnique();

  double * ct = new double[numTerms+1];

  int i;
  for (i=1; i<=numTerms; i++) ct[i]=0;

  int actualDocCount=0;
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
      ct[info->id()] += log(dm->seenProb(info->count(), info->id())/
			    (dm->unseenCoeff()*collectLM->prob(info->id())));
    }
    delete tList;
    delete dm;
  }
  if (actualDocCount==0) return;

  ArrayCounter<double> lmCounter(numTerms+1);
  
  double norm = 1.0/(double)actualDocCount;
  for (i=1; i<=numTerms; i++) { 
    lmCounter.incCount(i, exp(( ct[i]*norm -
				qryParam.fbMixtureNoise*log(collectLM->prob(i)))
			      / (1.0-qryParam.fbMixtureNoise)));
  }

  delete [] ct;

  
  MLUnigramLM *fblm = new MLUnigramLM(lmCounter, ind.termLexiconID());
  origRep.interpolateWith(*fblm, (1-qryParam.fbCoeff), qryParam.fbTermCount,
			qryParam.fbPrSumTh, qryParam.fbPrTh);
  delete fblm;
}

void SimpleKLRetMethod::computeMarkovChainFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs)
{
  int stopWordCutoff =50;

  ArrayCounter<double> *counter = new ArrayCounter<double>(ind.termCountUnique()+1);

  OneStepMarkovChain * mc = new OneStepMarkovChain(relDocs, ind, mcNorm,
						   1-qryParam.fbMixtureNoise);
  origRep.startIteration();
  double summ;
  while (origRep.hasMore()) {
    QueryTerm *qt;
    qt = origRep.nextTerm();
    summ =0;
    mc->startFromWordIteration(qt->id());
    // cout << " +++++++++ "<< ind.term(qt->id()) <<endl;
    int fromWd;
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

      counter->incCount(fromWd, (qt->weight()*fromWdPr*collectLM->prob(fromWd)/summ));
      // counter->incCount(fromWd, (qt->weight()*fromWdPr/summ));

    }
    delete qt;
  }
  delete mc;

  UnigramLM *fbLM = new MLUnigramLM(*counter, ind.termLexiconID());

  origRep.interpolateWith(*fbLM, 1-qryParam.fbCoeff, qryParam.fbTermCount,
			  qryParam.fbPrSumTh, qryParam.fbPrTh);
  delete fbLM;
  delete counter;
}


