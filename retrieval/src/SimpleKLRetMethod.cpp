#include "SimpleKLRetMethod.hpp"
#include "Param.hpp"
#include <iostream.h>
#include <fstream.h>
#include <math.h>
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

  // discounting the original model
  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight()*origModCoeff);
  }
  
  // now adding the new model
  double prSum = 0;
  int wdCount = 0;
  IndexedRealVector::iterator it;
  it = qm->begin();
  while (it != qm->end() && prSum < prSumThresh && wdCount < howManyWord && (*it).val >=prThresh) {
    cout << "==new-term== " << ind.term((*it).ind) << " "<< (*it).val << endl;
    incCount((*it).ind, (*it).val*(1-origModCoeff));
    it++;
    prSum += (*it).val;
    wdCount++;
  }
}

SimpleKLRetMethod::SimpleKLRetMethod(Index &dbIndex) 
{
  ind = &dbIndex;
  param.smthMethod = ParamGetInt("smoothMethod", JELINEKMERCER);
  param.smthStrategy= ParamGetInt("smoothStrategy", INTERPOLATE);

  param.ADDelta = ParamGetDouble("discountDelta",0.5);
  param.JMLambda = ParamGetDouble("JelinekMercerLambda",0.5);
  param.DirPrior = ParamGetDouble("BayesianPrior",2000);
  param.qFidelity = ParamGetDouble("queryFidelity",1);

  param.emIterations = ParamGetInt("emIterations",50);

  param.fbMethod = ParamGetInt("feedbackMethod",0);
  param.fbCoeff = ParamGetDouble("feedbackCoefficient",0.5);
  param.fbPrTh = ParamGetDouble("feedbackProbThresh",0.001);
  param.fbPrSumTh = ParamGetDouble("feedbackProbSumThresh",1);
  param.fbTermCount = ParamGetInt("feedbackTermCount",100);
  param.fbMixtureNoise = ParamGetDouble("feedbackNoise",0.5);
  ifstream ifs;
  ifs.open(ParamGetString("smoothSupportFile"));
  
  int numDocs = ind->docCount();

  docProbMass = new double[numDocs+1];
  uniqueTermCount = new int[numDocs+1];
  
  int i;
  
  for (i=0; i< numDocs; i++) {
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

  collectLMCounter = new DocUnigramCounter(*ind);
  //  collectLM = new MLUnigramLM(*collectLMCounter, ind->termLexiconID()); 
  collectLM = new LaplaceUnigramLM(*collectLMCounter, ind->termLexiconID(), 
				   ind->termCountUnique()); 

  scFunc = new SimpleKLScoreFunc();
}

SimpleKLRetMethod::~SimpleKLRetMethod() 
{
  delete [] docProbMass;
  delete [] uniqueTermCount;
  delete collectLM;
  delete collectLMCounter;
  delete scFunc;
}

DocumentRep *SimpleKLRetMethod::computeDocRep(int docID)
{
  switch (param.smthMethod) {
  case JELINEKMERCER:
    return( new JelinekMercerDocModel(docID,
				       ind, 
				       *collectLM,
				       docProbMass,
				       param.JMLambda,
				      param.smthStrategy));
  case BAYESIAN:
    return (new BayesianDocModel(docID,
				  ind, 
				  *collectLM,
				  docProbMass,  
				  param.DirPrior,
				 param.smthStrategy));
  case ABSOLUTEDISCOUNT:
    return (new AbsoluteDiscountDocModel(docID,
					  ind, 
					  *collectLM,
					  docProbMass,
					  uniqueTermCount,
					  param.ADDelta,
					 param.smthStrategy));
  default:
    cerr << "Unknown document language model smoothing method\n";
    exit(1);
  }

}


void SimpleKLRetMethod::updateQuery(QueryRep &origRep, DocIDSet &relDocs)
{
  SimpleKLQueryModel *qr = dynamic_cast<SimpleKLQueryModel *> (&origRep);
  
  UnigramLM *fbLM;
  DocUnigramCounter *dCounter;
  ArrayCounter<double> *counter;
  SimpleKLQueryModel *qm;

  switch ((QueryUpdateMethod)param.fbMethod) {
  case MAXLIKE:
    dCounter = new DocUnigramCounter(relDocs, *ind); 
    fbLM = new MLUnigramLM(*dCounter, ind->termLexiconID());
    qr->interpolateWith(*fbLM, (1-param.fbCoeff), param.fbTermCount,
			param.fbPrSumTh, param.fbPrTh);
    delete fbLM;
    delete dCounter;
    break;
  case MIXTURE:
    computeMixtureFBModel(*qr, relDocs);
    break;
    /* case DIVMIN:
    fbLM = new DivMinFBModel(relDocs, *ind, &collectLM, param.fbCoeff);
    qr->interpolateWith(*fbLM, (1-param.fbCoeff), param.fbTermCount,
			param.fbPrSumTh, param.fbPrTh);
    delete fbLM;
    break;
    */
  case MARKOVCHAIN:
    counter = new ArrayCounter<double>(ind->termCountUnique()+1);
    qm = dynamic_cast<SimpleKLQueryModel *> (&origRep);
    fbLM = computeMCQueryModel(*counter, *qm, relDocs, *ind, 500, param.fbCoeff);
    qr->interpolateWith(*fbLM, 0, param.fbTermCount,
			param.fbPrSumTh, param.fbPrTh);
    delete fbLM;
    delete counter;
    break;
  default:
    throw Exception("SimpleKLRetMethod", "unknown feedback method");
    break;
  }
}

MLUnigramLM *SimpleKLRetMethod::computeMCQueryModel(ArrayCounter<double> &counter,
						    SimpleKLQueryModel &origRep,
						    DocIDSet &relDocs,
						    Index &ind, 
						    int stopWordCutoff, 
						    double alpha)
{
  OneStepMarkovChain * mc = new OneStepMarkovChain(relDocs, ind, 
						   alpha);
  origRep.startIteration();
  while (origRep.hasMore()) {
    QueryTerm *qt;
    qt = origRep.nextTerm();
    double sum =0;
    mc->startFromWordIteration(qt->id());
    // cout << " +++++++++ "<< ind.term(qt->id()) <<endl;
    int fromWd;
    double fromWdPr;
    
    while (mc->hasMoreFromWord()) {
      mc->nextFromWordProb(fromWd, fromWdPr);
      if (fromWd <= stopWordCutoff) { // a stop word
	continue;
      }
      sum += fromWdPr*(mc->wordNorm())[fromWd];
      //      if (fromWdPr > 0.001) 
      // cout << ind.term(fromWd) << " " << fromWdPr << endl;
    }
    
    mc->startFromWordIteration(qt->id());
    while (mc->hasMoreFromWord()) {
      mc->nextFromWordProb(fromWd, fromWdPr);
      if (fromWd <= stopWordCutoff) { // a stop word
	continue;
      }
      counter.incCount(fromWd, qt->weight()*fromWdPr*(mc->wordNorm())[fromWd]/sum);
    }
    delete qt;
  }
  delete mc;
  return (new MLUnigramLM(counter, ind.termLexiconID()));
}


void SimpleKLRetMethod::computeMixtureFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs)
{
  int numTerms = ind->termCountUnique();

  DocUnigramCounter *dCounter = new DocUnigramCounter(relDocs, *ind);

  static double *distQuery = new double[numTerms+1];
  static double *distQueryEst = new double[numTerms+1];

  double noisePr;

  int j;
  int i;
  double bestLL ;
  double convergeLL;
  double distQueryNorm=0;

  for (i=1; i<=numTerms;i++) {
    distQueryEst[i] = rand()+0.001;
    distQueryNorm+= distQueryEst[i];
  }
  noisePr = param.fbMixtureNoise;

  int itNum = param.emIterations;
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
    cerr << "new likelihood = "<< ll << endl;
    if (itNum < param.emIterations && 
	ll - convergeLL < 0.001) {
      cerr << "converged at "<< param.emIterations - itNum+1 << " with "<< convergeLL << endl;
      break;
    } else {
      convergeLL = ll;
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
  MLUnigramLM *fblm = new MLUnigramLM(lmCounter, ind->termLexiconID());
  origRep.interpolateWith(*fblm, (1-param.fbCoeff), param.fbTermCount,
			param.fbPrSumTh, param.fbPrTh);
  delete fblm;
  delete dCounter;
}
