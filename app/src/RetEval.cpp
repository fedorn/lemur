 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/// A General Retrieval Evaluation Program



/*! \page RetrievalEvaluation Retrieval Evaluation Application

This application runs retrieval experiments (with/without feedback) to
evaluate different retrieval models as well as different parameter
settings for those models. 

It currently supports three different models:

1) The popular TFIDF retrieval model

2) The Okapi BM25 retrieval function

3) The KL-divergence language model based retrieval method

The parameter to select the model is "retModel" (with value 0 for
TFIDF, 1 for Okapi, and 2 for KL)

Other common parameters (for all retrieval methods)  are:


1) "index": the index table-of-content file for the database index. It should have a file extension with exactly three characters. The convention assumed is
".lmr" or ".LMR" means the basic indexer and ".toc" or ".TOC" means the InvFP indexer. (These extensions should be modified to reflect the characteristics of the corresponding indexer, but this has not been done yet.)

2) "querySet": the query text stream 

3) "resultFile": the result file

4) "resultCount": the number of documents to return for each query

5) "feedbackDocCount": the number of docs to use for pseudo-feedback (0 means no-feedback)

6) "feedbackTermCount": the number of terms to add to a query when doing feedback


Other model-specific parameters are:

For TFIDF:

1) "feedbackPosCoeff": the coefficient for positive terms in Rocchio feedback

2) "doc.tfMethod": document term TF weighting method

3) "doc.bm25K1": BM25 k1 for doc term TF

4) "doc.bm25B" : BM25 b for doc term TF

5) "query.tfMethod": query term TF weighting method

6) "query.bm25K1": BM25 k1 for query term TF


For Okapi:

1) "BM25K1" : BM25 K1

2) "BM25B" : BM25 B

3) "BM25AvgLen": BM25 avg doc length 

4) "BM25K3": BM25 K3 

5) "BM25QTF": The TF for expanded terms in feedback (the original paper
about the Okapi system is not clear about how this is set, so
it's implemented as a parameter.)

For KL-divergence:

1) "smoothMethod": language model smoothing method 
(0,1,2 for Jelinek-Mercer, Dirichlet-Prior, and Absolute-discounting respectively)

2) "discountDelta": delta value for additive smoothing

3) "JelinekMercerLambda": lambda value for Jelinek-Mercer smoothing

4) "BayesianPrior": prior count for Bayesian smoothing

5) "smoothStrategy": interpolation (0) vs. backoff (1)

6) "queryFidelity": topic weight for the query mixture model (i.e., 
two-stage smoothing  (1 means no collection model mixed, so essentially the query-likelihood model)

7) "feedbackMethod": feedback method (0, 1, 2, 3 for maximum-likelihood, mixture model, divergence minimization, and Markov chain respectively).

8) Other feedback related parameters (to be added later).

 */



#include "Param.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include <iostream.h>
#include "TFIDFRetMethod.hpp"
#include "SimpleKLRetMethod.hpp"
#include "OkapiRetMethod.hpp"
#include "RetrievalEngine.hpp"

#include "ParamManager.hpp"
#include "ResultFile.hpp"
namespace LocalParameter {
  enum RetModel {TFIDF=0, OKAPI=1, KL=2};
  /// retrieval model 
  static enum RetModel mod;
  static bool TRECResultFormat;
  void get() {
    mod = (RetModel) ParamGetInt("retModel",KL); // default is KL divergence model
    TRECResultFormat = ParamGetInt("resultFormat",1); // default is TREC format
  }
};

void GetAppParam()
{
  RetrievalParameter::get();
  LocalParameter::get();
}



/// A retrieval evaluation program
int AppMain(int argc, char *argv[]) {
  
  Index  *ind = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  DocStream *qryStream = new BasicDocStream(RetrievalParameter::textQuerySet);
  ofstream result(RetrievalParameter::resultFile);

  ResultFile resFile(LocalParameter::TRECResultFormat);

  resFile.openForWrite(result, *ind);

  IndexedRealVector *results;

  RetrievalMethod *model;

  // construct retrieval model

  switch (LocalParameter::mod) {
  case LocalParameter::TFIDF: 
    model = new TFIDFRetMethod(*ind);
    TFIDFParameter::get();
    ((TFIDFRetMethod *)model)->setDocTFParam(TFIDFParameter::docTFPrm);
    ((TFIDFRetMethod *)model)->setQueryTFParam(TFIDFParameter::qryTFPrm);
    ((TFIDFRetMethod *)model)->setFeedbackParam(TFIDFParameter::fbPrm);
    break;
  case LocalParameter::OKAPI:
    model = new OkapiRetMethod(*ind);
    OkapiParameter::get();
    ((OkapiRetMethod *)model)->setTFParam(OkapiParameter::tfPrm);
    ((OkapiRetMethod *)model)->setFeedbackParam(OkapiParameter::fbPrm);
    break;
  case LocalParameter::KL:
    SimpleKLParameter::get();
    model = new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile);
    ((SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
    ((SimpleKLRetMethod *)model)->setQueryModelParam(SimpleKLParameter::qryPrm);
    break;
  default:
    throw Exception("RetrievalExp", "unknown retModel parameter");
    break;
    
  }

  RetrievalEngine eng(*model);
  qryStream->startDocIteration();
  TextQuery *q;
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = (TextQuery *) d;
    cout << "query : "<< q->getID() << endl;
    if (RetrievalParameter::fbDocCount>0) {
      eng.retrievePseudoFeedback(*q, RetrievalParameter::fbDocCount, results);
    } else {
      eng.retrieve(*q, results);
    }
    resFile.writeResults(q->getID(), results, RetrievalParameter::resultCount);
  }

  result.close();
  delete qryStream;
  delete ind;
  return 0;

}


