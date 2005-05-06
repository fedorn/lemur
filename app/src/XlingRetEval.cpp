/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
/*
  author: dmf
 */

/*! \page Xling Cross Lingual Retrieval Evaluation

<p> This application runs cross-lingual retrieval
experiments.

Parameters are:

<p>
<ol>
<li> <tt>sourceIndex</tt>: The complete name of the index for the source 
language collection. This provides the background model for the source 
language.
<li> <tt>targetIndex</tt>: The complete name of the index for the target
language collection. This is the collection that is searched.

<li> <tt>textQuery</tt>: the query text stream, in the source language
<li> <tt>XLlambda</tt>: The smoothing parameter for mixing P(t|D) and P(s|GS).
<li> <tt>XLbeta</tt>: The Jelinik-Mercer lambda for estimating P(t|D).

<li> <tt>sourceBackgroundModel</tt>: One of "term" or "doc". If term,
background model for the source language is estimated as tf(s)/|V|. If
doc, the background model for the source language is estimated as
df(t)/sum_w_in_V df(w). Default is term.

<li> <tt>targetBackgroundModel</tt>: One of "term" or "doc". If term,
background model for the target language is estimated as tf(s)/|V|. If
doc, the background model for the target language is estimated as
df(t)/sum_w_in_V df(w). Default is term.

<li> <tt>resultFile</tt>: the result file
<li> <tt>resultFormat</tt>: whether the result format should be of the 
TREC format (i.e., six-column) or just a simple three-column format 
<tt>&lt;queryID, docID, score&gt;</tt>. String value, either <tt>trec</tt>
for TREC format or <tt>3col</tt> for three column format. Default: TREC 
format.
<li> <tt>resultCount</tt>: the number of documents to return for each query 

<li> <tt>feedbackDocCount</tt>: the number of docs to use for
pseudo-feedback (0 means no-feedback) 
<li> <tt>feedbackTermCount</tt>: the number of terms to add to a query
when doing feedback. 
</ol>

<P>
Simple KL parameters:
<ol>
<li> <tt>smoothSupportFile</tt>: The name of the smoothing support file
<li> <tt>smoothMethod</tt>: One of the four: 
<ul>
<li><tt>jelinikmercer</tt> or <tt>jm</tt> for Jelinek-Mercer
<li><tt>dirichletprior</tt> or <tt>dir</tt> for Dirichlet prior 
<li><tt>absolutediscount</tt> or <tt>ad</tt> for Absolute discounting
<li><tt>twostage</tt> or <tt>2s</tt> for two stage.
</ul>

<li> <tt>smoothStrategy</tt>: Either <tt>interpolate</tt> for
interpolate or <tt>backoff</tt> for backoff.

<li> <tt>adjustedScoreMethod</tt>: Which type of score to output, one
of: 
<ul>
<li> "querylikelihood" or "ql" for query likelihood.
<li> "crossentropy" or "ce" for cross entropy.
<li> "negativekld" or "-d" for negative KL divergence.
</ul>

<li> <tt>JelinekMercerLambda</tt>: The collection model weight in the JM
interpolation method. Default: 0.5 

<li> <tt>DirichletPrior</tt>: The prior parameter in the Dirichlet prior
smoothing method. Default: 1000 

<li> <tt>discountDelta</tt>: The delta (discounting constant) in the
absolute discounting method. Default 0.7. 
<li> <tt>queryUpdateMethod</tt>: feedback method, one of:
<ul>
<li><tt>relevancemodel1</tt> or <tt>rm1</tt> for relevance model 1.
<li><tt>relevancemodel2</tt> or <tt>rm2</tt> for relevance model 2.
</ul>
</ol>
<ol>
<li> <tt>feedbackCoefficient</tt>: the coefficient of the feedback model
for interpolation. The value is in [0,1], with 0 meaning using only the
original model (thus no updating/feedback) and 1 meaning using only the
feedback model (thus ignoring the original model). 

<li> <tt>feedbackTermCount</tt>: Truncate the feedback model to no more
than a given number of words/terms. 

<li> <tt>feedbackProbThresh</tt>: Truncate the feedback model to include
only words with a probability higher than this threshold. Default value:
0.001. 

<li> <tt>feedbackProbSumThresh</tt>: Truncate the feedback model until
the sum of the probability of the included words reaches this
threshold. Default value: 1. 
</ol>
Parameters <tt>feedbackTermCount</tt>, <tt>feedbackProbThresh</tt>, and
<tt>feedbackProbSumThresh</tt> work conjunctively to control the
truncation, i.e., the truncated model must satisfy all the three
constraints.  

 */

#include "common_headers.hpp"
#include "PDict.hpp"
#include "IndexManager.hpp"
#include "RetMethodManager.hpp"
#include "XLingRetMethod.hpp"
#include "BasicDocStream.hpp"
#include "ResultFile.hpp"
using namespace lemur::api;

/// cross lingual retrieval model parameters
// might want to change and expand here.
namespace XLingParameter {
  /// default lambda value for xling
  double lambda = 0.7;
  double beta = 1.0;
  string sourceIndex, targetIndex;
  string dictionary;
  string sourceBackgroundModel;
  string targetBackgroundModel;
  
  void get() {
    lambda = ParamGetDouble("XLlambda", lambda);
    beta = ParamGetDouble("XLbeta", beta); // target smooth param.
    sourceIndex = ParamGetString("sourceIndex", "");
    targetIndex = ParamGetString("targetIndex", "");
    dictionary = ParamGetString("dictionary", "");
    sourceBackgroundModel = ParamGetString("sourceBackgroundModel", "term");
    targetBackgroundModel = ParamGetString("targetBackgroundModel", "term");
  }
};

void GetAppParam()
{
  RetrievalParameter::get();
  XLingParameter::get();
}


int AppMain(int argc, char *argv[]) {
  Index *sInd, *tInd;
  lemur::retrieval::XLingRetMethod *model;
  lemur::retrieval::SimpleKLRetMethod *klModel;
  lemur::dictionary::PDict dict;
  dict.open(XLingParameter::dictionary);
  sInd = IndexManager::openIndex(XLingParameter::sourceIndex.c_str());
  tInd = IndexManager::openIndex(XLingParameter::targetIndex.c_str());
  
  lemur::retrieval::ArrayAccumulator acc(tInd->docCount());
  lemur::retrieval::ArrayAccumulator klAcc(tInd->docCount());

  // Create the stopper if needed.
  Stopper * stopper = NULL;
  stopper = TextHandlerManager::createStopper();

  // Create the stemmer if needed.
  Stemmer * stemmer = NULL;
  stemmer = TextHandlerManager::createStemmer();

  model = new lemur::retrieval::XLingRetMethod(*tInd, *sInd, dict, acc, 
                                               XLingParameter::lambda,
                                               XLingParameter::beta,
                                               RetrievalParameter::cacheDocReps,
                                               XLingParameter::sourceBackgroundModel,
                                               XLingParameter::targetBackgroundModel,
                                               stopper, stemmer);
  bool ignoreWeights = false; // RM1 -- no ignore

  ofstream result(RetrievalParameter::resultFile.c_str());
  ResultFile resFile(RetrievalParameter::TRECresultFileFormat);
  resFile.openForWrite(result, *tInd);

  IndexedRealVector results(tInd->docCount());
  DocStream *qryStream;
  qryStream = new lemur::parse::BasicDocStream(RetrievalParameter::textQuerySet);
  qryStream->startDocIteration();
  TextQuery *q;
  IndexedRealVector::iterator j;
  string retModel("kl"); // force kl

  if (RetrievalParameter::fbDocCount > 0) {
    klModel = (lemur::retrieval::SimpleKLRetMethod *) RetMethodManager::createModel(tInd, 
                                                                  &klAcc, 
                                                                  retModel);
  } else {
    klModel = NULL;
  }

  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : "<< q->id() << endl;
    QueryRep * qr = model->computeQueryRep(*q);
    //model->scoreInvertedIndex(*qr, results);
    model->scoreCollection(*qr, results);
    results.Sort();
    if (RetrievalParameter::fbDocCount > 0) {
      // prune to number of feedback docs.
      if (results.size() > RetrievalParameter::fbDocCount)
        results.erase(results.begin() + RetrievalParameter::fbDocCount,
                      results.end());
      int n = dynamic_cast<lemur::retrieval::XLingQueryModel *>(qr)->getNumTerms();
      double scale = 1.0/n;
      // weigh by 1/n to match lm
      for (j = results.begin(); j != results.end(); j++)
        (*j).val *= scale;
      
      results.LogToPosterior();
      results.Sort();
      // RM1 or RM2  expansion and then monolingual kl
      PseudoFBDocs *topDoc = new PseudoFBDocs(results, 
                                              RetrievalParameter::fbDocCount, 
                                              ignoreWeights);
      // get the translations
      QueryRep * klRep = model->computeTargetKLRep(qr); 
      klModel->updateQuery(*klRep, *topDoc);
      //dbg
      //  os << q->id();
      //      ((SimpleKLQueryModel *)klRep)->save(os);
      klModel->scoreCollection(*klRep, results);
      results.Sort();
      delete(klRep);
      delete(topDoc);
    }
    resFile.writeResults(q->id(), &results, RetrievalParameter::resultCount);
    delete(qr);
    delete(q);
  }
  dict.close();
  result.close();
  delete(qryStream);
  delete(model);
  delete(klModel);
  delete(sInd);
  delete(tInd);
  delete(stopper);
  delete(stemmer);
  return 0;
}
