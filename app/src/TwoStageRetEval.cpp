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




/// Retrieval Evaluation Using Two-stage Smoothing Method



/*! \page TwoStageRetEval Two-stage Smoothing Retrieval Evaluation Application
<p>
This application (TwoStageRetEval.cpp) runs retrieval experiments (with/without feedback) in exactly the same way as the application <tt>RetEval.cpp</tt>, 
except that it always uses the two-stage smoothing method for 
the initial retrieval and the KL-divergence model for feedback. 
It thus ignores the the parameter <tt>retModel</tt>. 

<p>
It recognizes all the parameters relevant to the KL-divergence retrieval model,
except for the smoothing method parameter <tt>SmoothMethod</tt> which is
forced to the "Two-stage Smoothing" (value of 3) and <tt>JelinekMercerLambda</tt>, which gets ignored, since it automatically estimates the value of <tt>JelinekMercerLambda</tt> using a mixture model. For details on all the parameters, see the documentation for <tt>RetEval.cpp</tt>. 

<p> 
To achieve the effect of 
the completely automatic two-stage smoothing method, the parameter
<tt>DirichletPrior</tt> should be set to the estimated value of the Dirichlet
prior smoothing parameter using the application <tt>EstimateDirPrior</tt>,
which computes a Maximum Likelihood estimate of <tt>DirichletPrior</tt>
based on "leave-one-out". 

 */



#include "Param.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include "SimpleKLRetMethod.hpp"
#include "RetParamManager.hpp"
#include "ResultFile.hpp"

namespace LocalParameter {
  enum RetModel {TFIDF=0, OKAPI=1, KL=2};
  /// retrieval model 
  static bool TRECResultFormat;
  bool useWorkingSet;
  String workSetFile;
  void get() {
    TRECResultFormat = ParamGetInt("resultFormat",1); // default is TREC format
    useWorkingSet = ParamGetInt("useWorkingSet", 0); //default is to score the whole collection; otherwise, score a subset
    workSetFile = ParamGetString("workingSetFile",""); // working set file name
  }
};

void GetAppParam()
{
  RetrievalParameter::get();
  LocalParameter::get();
}


double estimateQueryNoise(QueryRep *qrep, Index *ind)
{
  SimpleKLQueryModel *qr = (SimpleKLQueryModel *)qrep;


  int dMax = ind->docCount();
  double *lambdaEst = new double[dMax+1];
  double *z = new double[dMax+1];

  double *pi = new double[dMax+1];
  double lambda;

  int tMax = ind->termCountUnique();

  double mu = SimpleKLParameter::docPrm.DirPrior;

  lambda = 0.5;


  double curFit, meanFit;
  meanFit = 1e-40;
  
  int d;
  for (d=1; d<=dMax; d++) {
    pi[d] = 1.0/(double)dMax;
    
  }


  int it = 0;
  do { // EM
    it++;
    curFit = 0;

    // initialize counters
    for (d=1; d<=dMax; d++) {
      z[d] = log(pi[d]);
      lambdaEst[d] = 0;
    }
    // compute z
    qr->startIteration();
    double qSum = 0;
    double lnorm = 0;
    while (qr->hasMore()) {
      QueryTerm *qt = qr->nextTerm();
      int tid = qt->id();
      double qfq = qt->weight();

      double cpr = (1+ind->termCount(tid))/(double)(ind->termCount()+tMax); 
      lnorm += qfq*log(lambda*cpr);

      qSum += qfq;

      // default value when not matching the term
      for (d=1; d<=dMax; d++) {
	int docLen = ind->docLength(d);
	z[d] += qfq*log(1+ (1-lambda)*mu/(lambda*(docLen+mu)));
	lambdaEst[d] += qfq*(docLen+mu)*lambda/(lambda*docLen+mu);
      }
      
      DocInfoList *dList = ind->docInfoList(tid);
      dList->startIteration();
      while (dList->hasMore()) {
	    
	DocInfo *info = dList->nextEntry();
	double dlen = ind->docLength(info->docID());
	double dpr = (info->termCount()+mu*cpr)/(double)(dlen+mu);
	z[info->docID()] += qfq*(log((1-lambda)*dpr/(lambda*cpr)+1)-
	  log(1+ (1-lambda)*mu/(lambda*(dlen+mu))));
	
	lambdaEst[info->docID()] += qfq*(lambda*cpr/(lambda*cpr+(1-lambda)*dpr)-(dlen+mu)*lambda/(lambda*dlen+mu));
      
      }
      delete dList;
      delete qt;
    }

    double zNorm = 0;
    for (d=1; d<=dMax; d++) {
      z[d] = exp(z[d]);
      zNorm += z[d];
    }
    zNorm = 1.0/zNorm;

    // parameter re-estimation
    double tempLambda = 0;
    for (d=1; d<=dMax; d++) {
      z[d] *= zNorm;
      tempLambda += z[d]*lambdaEst[d];
      pi[d] = z[d];

    }

    lambda = tempLambda/qSum;
    curFit = -log(zNorm) + lnorm;
    meanFit = 0.5*meanFit+0.5*curFit;
    cout << "*** current fit:"<< curFit << " lambda = "<< lambda << endl;
  } while (fabs((meanFit-curFit)/meanFit)>0.0001 && it < 10);
  delete [] z;
  delete [] pi;
  delete lambdaEst;
  return lambda;
}

/// A retrieval evaluation program
int AppMain(int argc, char *argv[]) {
  
  Index  *ind = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  DocStream *qryStream = new BasicDocStream(RetrievalParameter::textQuerySet);

  ofstream result(RetrievalParameter::resultFile);
  ResultFile resFile(LocalParameter::TRECResultFormat);
  resFile.openForWrite(result, *ind);

  ifstream workSetStr(LocalParameter::workSetFile);
  ResultFile docPool(false); // working set is always simple format
  docPool.openForRead(workSetStr, *ind);

  ArrayAccumulator accumulator(ind->docCount());

  IndexedRealVector results(ind->docCount());

  RetrievalMethod *model;

  // construct retrieval model

  SimpleKLParameter::get();
  SimpleKLParameter::docPrm.smthMethod = SimpleKLParameter::TWOSTAGE;
  // force two-stage smoothing

  model = new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile, accumulator);
  ((SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
  ((SimpleKLRetMethod *)model)->setQueryModelParam(SimpleKLParameter::qryPrm);


  qryStream->startDocIteration();
  TextQuery *q;
  
  IndexedRealVector workSetRes;
  
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : "<< q->id() << endl;
    QueryRep * qr = model->computeQueryRep(*q);
    
    double qn = estimateQueryNoise(qr, ind);

    cout << "### Final lambda for "<< q->id() << " = "<< qn << endl;
    
     SimpleKLParameter::docPrm.JMLambda =qn; 
     ((SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
    PseudoFBDocs *workSet;

    if (LocalParameter::useWorkingSet) {
      docPool.getResult(q->id(), workSetRes);
      workSet = new PseudoFBDocs(workSetRes, -1); // -1 means using all docs
      model->scoreDocSet(*qr,*workSet,results);
    } else {
      model->scoreCollection(*qr, results);
    }

    results.Sort();
    if (RetrievalParameter::fbDocCount>0) {
      PseudoFBDocs *topDoc = new PseudoFBDocs(results, RetrievalParameter::fbDocCount);
      model->updateQuery(*qr, *topDoc);
     
      if (LocalParameter::useWorkingSet) {
	model->scoreDocSet(*qr,*workSet,results);
      } else {
	model->scoreCollection(*qr, results);
      } 
      results.Sort();
      delete topDoc;
    }
    resFile.writeResults(q->id(), &results, RetrievalParameter::resultCount);
    
    if (LocalParameter::useWorkingSet) {
      delete workSet;
    }
    delete qr;
    delete q;
  }

  delete model;
  result.close();
  delete qryStream;
  delete ind;
  return 0;

}


