
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

String inputIndex;
String inputQuerySet;
String inputResultFile;



enum RetModel {TFIDF=0, OKAPI=1, KL=2};


void GetAppParam()
{
 inputIndex = ParamGetString("index");
 inputQuerySet = ParamGetString("querySet", "query");
 inputResultFile = ParamGetString("resultFile", "result");
}

static void writeResults(char *queryID, IndexedRealVector *results, Index *ind, int maxCountOfResult, ostream *resStream)
{
  IndexedRealVector::iterator j;
  int count=0;
  for (j= results->begin();j!=results->end();j++) {
    if (count >= maxCountOfResult) {
      break;
    } else {
      *resStream << queryID << " "  
	  << ind->document((*j).ind) << " " 
	  <<  (*j).val << endl;
      count++;

    }
  }  
  resStream->flush();
}



/// A retrieval evaluation program
int AppMain(int argc, char *argv[]) {
  
  Index  *ind = IndexManager::openIndex(inputIndex);
  DocStream *qryStream = new BasicDocStream(inputQuerySet);
  ofstream result(inputResultFile);

  int mod = ParamGetInt("retModel",3); // default is KL divergence model

  int fbDocCount = ParamGetInt("feedbackDocCount",0); 
  int maxCountOfResult = ParamGetInt("resultCount", 1000);

  IndexedRealVector *results;

  RetrievalMethod *model;

  // construct retrieval model

  switch (mod) {
  case 0: 
    model = new TFIDFRetMethod(*ind);
    break;
  case 1:
    model = new OkapiRetMethod(*ind);
    break;
  case 2:
    model = new SimpleKLRetMethod(*ind);
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
    if (fbDocCount>0) {
      eng.retrievePseudoFeedback(*q, fbDocCount, results);
    } else {
      eng.retrieve(*q, results);
    }
    writeResults(q->getID(), results, ind, maxCountOfResult, &result);
  }

  result.close();
  delete qryStream;
  delete ind;
  return 0;
}


