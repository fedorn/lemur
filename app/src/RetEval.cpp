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




/// A General Retrieval Evaluation Program



/*! \page RetrievalEvaluation Retrieval Evaluation Application
<p>
This application (RetEval.cpp) runs retrieval experiments (with/without feedback) to
evaluate different retrieval models as well as different parameter
settings for those models. 
<p>
Scoring is either done over a working set of documents (essentially re-ranking), or over the whole collection. This is indicated by the parameter "useWorkingSet". When "useWorkingSet" has a non-zero (integer) value, scoring will
be on a working set specified in a file given by "workSetFile". The file
should have three columns. The first is the query id; the second the document id; and the last a numerical value, which is ignored. The reason for having a third column of numerical values is so that any retrieval result of the simple format (i.e., non-trec format) generated by Lemur could be directly used as a "workSetFile" for the purpose of re-ranking, which is convenient. Also, the third
column could be used to provide a prior probability value for each document, which could be useful for some algorithms. By default, scoring is on the whole collection.
<p>
It currently supports three different models:
<ol>
<li> The popular TFIDF retrieval model

<li>The Okapi BM25 retrieval function

<li> The KL-divergence language model based retrieval method
</ol>
<p>
The parameter to select the model is <tt>retModel</tt> (with value 0 for
TFIDF, 1 for Okapi, 2 for KL, and 3 for CORI (inquery)). It is suspected that there is a bugin the implementation of the feedback for Okapi BM25 retrieval function, because the performance is not as expected. 
<p>
Other common parameters (for all retrieval methods)  are:
<p>
<ol>
<li> <tt>index</tt>: The complete name of the index table-of-content file for the database index.

<li> <tt>textQuerySet</tt>: the query text stream 

<li> <tt>resultFile</tt>: the result file

<li> <tt>resultCount</tt>: the number of documents to return as result for each query

<li> <tt>feedbackDocCount</tt>: the number of docs to use for pseudo-feedback (0 means no-feedback)

<li> <tt>feedbackTermCount</tt>: the number of terms to add to a query when doing feedback. Note that
    in the KL-div. approach, the actual number of terms is also affected by two other parameters.(See below.)
</ol>
<P>
Model-specific parameters are:
<ul>
<LI>For TFIDF:
<ol>
<li> <tt>feedbackPosCoeff</tt>: the coefficient for positive terms in (positive) Rocchio feedback. We only implemented the positive part and non-relevant documents are ignored.
<li> <tt>doc.tfMethod</tt>: document term TF weighting method: 0 for <tt>RawTF</tt>, 1 for <tt>log-TF</tt>, and 2 for <tt>BM25TF</tt>

<li> <tt>doc.bm25K1</tt>: BM25 k1 for doc term TF

<li> <tt>doc.bm25B</tt> : BM25 b for doc term TF

<li> <tt>query.tfMethod</tt>: query term TF weighting method: 0 for <tt>RawTF</tt>, 1 for <tt>log-TF</tt>, and 2 for <tt>BM25TF</tt>

<li><tt>query.bm25K1</tt>: BM25 k1 for query term TF. bm25B is set to zero for query terms
</ol>
<li>For Okapi:
<ol>
<li> <tt>BM25K1</tt> : BM25 K1

<li> <tt>BM25B</tt> : BM25 B

<li> <tt>BM25K3</tt>: BM25 K3 

<li> <tt>BM25QTF</tt>: The TF for expanded terms in feedback (the original paper
about the Okapi system is not clear about how this is set, so
it's implemented as a parameter.)
</ol>

<li>For CORI:
<ol>
<li><tt>TF_factor</tt>
<li><tt>TF_baseline</tt>
<li> <tt>collCounts</tt> - Use value "USE_INDEX_COUNTS" to use counts from the index if no separate collection counts file is available.  For collection selection indexes built from collSell application, that file is auto generated.
</ol>
<li>For KL-divergence:
<ol>

<p>
<B>Document model smoothing parameters:</B>
<li> <tt>smoothSupportFile</tt>: The name of the smoothing support file (e.g., one generated by GenerateSmoothSupport).

<li> <tt>smoothMethod</tt>: One of the three: Jelinek-Mercer (0), Dirichlet prior (1), and Absolute discounting (2)

<li> <tt>smoothStrategy</tt>: Either <tt>interpolate</tt> (0) or <tt>backoff</tt> (1)

<li> <tt>JelinekMercerLambda</tt>: The collection model weight in the JM interpolation method. Default: 0.5

<li> <tt>DirichletPrior</tt>: The prior parameter in the Dirichlet prior smoothing method. Default: 1000

<li> <tt>discountDelta</tt>: The delta (discounting constant) in the absolute discounting method. Default 0.7.
<p>
<b>Query model updating method (i.e., pseudo feedback):</b>
<li> <tt>queryUpdateMethod</tt>: feedback method (0, 1, 2 for mixture model, divergence minimization, and Markov chain respectively).

<li>  Method-specific feedback parameters:
<p>
For all interpolation-based approaches (i.e., the new query model is an interpolation of the original
model with a (feedback) model computed based on the feedback documents), the following four
parameters apply:
<ol>
<li> <tt>feedbackCoefficient</tt>: the coefficient of the feedback model for interpolation. The value is in [0,1], with 0 meaning using only the original model (thus no updating/feedback) and 1 meaning using only the feedback model (thus ignoring the original model).

<li> <tt>feedbackTermCount</tt>: Truncate the feedback model to no more than a given number of words/terms.

<li> <tt>feedbackProbThresh</tt>: Truncate the feedback model to include only words with a probability higher than this threshold. Default value: 0.001.

<li> <tt>feedbackProbSumThresh</tt>: Truncate the feedback model until the sum of the probability of the included words reaches this threshold. Default value: 1.
<p>
</ol>
Parameters <tt>feedbackTermCount</tt>, <tt>feedbackProbThresh</tt>, and <tt>feedbackProbSumThresh</tt> work conjunctively to control the truncation, i.e., the truncated model must satisfy all the three constraints. 
<p>
All the three feedback methods also recognize the parameter <tt>feedbackMixtureNoise</tt> (default value :0.5), but with <font color=red> <em> different</em> interpretations</font>.  
<ul>
<li> For the collection mixture model method, <tt>feedbackMixtureNoise</tt> is the collection model selection probability in the mixture model. That is, with this probability, a word is picked according to the collection language model, when a feedback document is "generated".
<li> For the divergence minimization method, <tt>feedbackMixtureNoise</tt> means
the weight of the divergence from the collection language model. (The higher it is, the farther the estimated model is from the collection model.)
<li> For the Markov chain method, <tt>feedbackMixtureNoise</tt> is the probability
of <em>not</em> stopping, i.e., <tt>1- alpha</tt>, where alpha is the stopping probability while walking through the chain. 
</ul>
<p>
In addition, the collection mixture model also recognizes the parameter 
<tt>emIterations</tt>, which is the maximum number of iterations the EM algorithm will run. Default: 50. (The EM algorithm can terminate earlier if the log-likelihood converges quickly, where convergence is measured by some hard-coded criterion. See the source code in <tt>SimpleKLRetMethod.cpp</tt> for details. )

</ol>
</ul>

 */



#include "Param.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include "common_headers.hpp"
#include "TFIDFRetMethod.hpp"
#include "SimpleKLRetMethod.hpp"
#include "OkapiRetMethod.hpp"
#include "CORIRetMethod.hpp"
#include "RetParamManager.hpp"
#include "ResultFile.hpp"

namespace LocalParameter {
  enum RetModel {TFIDF=0, OKAPI=1, KL=2, CORIDOC=3};
  /// retrieval model 
  static enum RetModel mod;
  static bool TRECResultFormat;
  bool useWorkingSet;
  String workSetFile;
  void get() {
    mod = (RetModel) ParamGetInt("retModel",KL); // default is KL divergence model
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



/// A retrieval evaluation program
int AppMain(int argc, char *argv[]) {
  
  Index  *ind;

  try {
    ind  = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("RelEval", "Can't open index, check parameter index");
  }

  
  DocStream *qryStream;
  try {
    qryStream = new BasicDocStream(RetrievalParameter::textQuerySet);
  } 
  catch (Exception &ex) {
    ex.writeMessage(cerr);
    throw Exception("RetEval", "Can't open query file, check parameter textQuery");
  }



  ofstream result(RetrievalParameter::resultFile);
  ResultFile resFile(LocalParameter::TRECResultFormat);
  resFile.openForWrite(result, *ind);

  ifstream *workSetStr;
  ResultFile *docPool;
  if (LocalParameter::useWorkingSet) {
    workSetStr = new ifstream(LocalParameter::workSetFile, ios::in);
    if (workSetStr->fail()) {
      throw Exception("RetEval", "can't open working set file");
    }
    docPool = new ResultFile(false); // working set is always simple format
    docPool->openForRead(*workSetStr, *ind);
  }

  ArrayAccumulator accumulator(ind->docCount());

  IndexedRealVector results(ind->docCount());

  RetrievalMethod *model;

  // construct retrieval model

  switch (LocalParameter::mod) {
  case LocalParameter::TFIDF: 
    model = new TFIDFRetMethod(*ind, accumulator);
    TFIDFParameter::get();
    ((TFIDFRetMethod *)model)->setDocTFParam(TFIDFParameter::docTFPrm);
    ((TFIDFRetMethod *)model)->setQueryTFParam(TFIDFParameter::qryTFPrm);
    ((TFIDFRetMethod *)model)->setFeedbackParam(TFIDFParameter::fbPrm);
    break;
  case LocalParameter::OKAPI:
    model = new OkapiRetMethod(*ind, accumulator);
    OkapiParameter::get();
    ((OkapiRetMethod *)model)->setTFParam(OkapiParameter::tfPrm);
    ((OkapiRetMethod *)model)->setFeedbackParam(OkapiParameter::fbPrm);
    break;
  case LocalParameter::KL:
    SimpleKLParameter::get();
    model = new SimpleKLRetMethod(*ind, SimpleKLParameter::smoothSupportFile, accumulator);
    ((SimpleKLRetMethod *)model)->setDocSmoothParam(SimpleKLParameter::docPrm);
    ((SimpleKLRetMethod *)model)->setQueryModelParam(SimpleKLParameter::qryPrm);
    break;
  case LocalParameter::CORIDOC:
    CORIParameter::get();
    model = new CORIRetMethod(*ind, accumulator, CORIParameter::collectionCounts);
    ((CORIRetMethod*)model)->setTFFactor(CORIParameter::doctffactor);
    ((CORIRetMethod*)model)->setTFBaseline(CORIParameter::doctfbaseline);    
    break;
  default:
    throw Exception("RetEval", "unknown retModel parameter");
    break;
    
  }

  qryStream->startDocIteration();
  TextQuery *q;
  
  IndexedRealVector workSetRes;
  
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : "<< q->id() << endl;
    QueryRep * qr = model->computeQueryRep(*q);
    PseudoFBDocs *workSet;

    if (LocalParameter::useWorkingSet) {
      docPool->getResult(q->id(), workSetRes);
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

  if (LocalParameter::useWorkingSet) {
    delete docPool;
    delete workSetStr;
  }
  delete model;
  result.close();
  delete qryStream;
  delete ind;
  return 0;

}

