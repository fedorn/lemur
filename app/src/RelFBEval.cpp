/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

/*! \page RelFBEval Relevance Feedback Evaluation Application

<p> This application runs retrieval experiments with relevance
feedback. Different retrieval models can be used with different settings
for the corresponding parameters. Although this program is designed for
relevance feedback, it can be easily used for pseudo feedback -- you
just need to set the parameter <tt> feedbackDocuments</tt> to a result
file, i.e., interpreting a result file as if all the entries represent
relevant documents.

<p>
Two important notes:
<ul>

<li> All the feedback algorithms currently in Lemur assume that all
entries in a judgment file are <em> relevant </em> documents, so you
must remove all the entries of judged non-relevant documents. However,
the judgment status is recorded in the internal representation of
judgments, so that it is possible to distinguish judged relevant
documents from judged non-relevant documents in a feedback algorithm.

<li> The format of the judgment file, when used for feedback, must be of
three columns, i.e., with the second column removed so that each line
has a query id, a document id, and a judgment value. This is to be
consistent with the format of a result file. An alternative would be to
use the original four-column format directly, but, then we would need to
add a parameter to distinguish this four-column format from the
three-column format of a result file.

</ul>

<p>
Scoring is either done over a working set of documents (essentially
re-ranking), or over the whole collection. This is indicated by the
parameter "useWorkingSet". When "useWorkingSet" has either a non-zero
(integer) value or the value <tt>true</tt>, scoring will be on a working set
specified in a file given by "workingSetFile". The file should have three
columns. The first is the query id; the second the document id; and the
last a numerical value, which is ignored. The reason for having a third
column of numerical values is so that any retrieval result of the simple
format (i.e., non-trec format) generated by Lemur could be directly used
as a "workingSetFile" for the purpose of re-ranking, which is
convenient. Also, the third column could be used to provide a prior
probability value for each document, which could be useful for some
algorithms. By default, scoring is on the whole collection.

<p>It currently supports five different models:
<ol>
<li> The popular TFIDF retrieval model
<li> The Okapi BM25 retrieval function
<li> The KL-divergence language model based retrieval method
<li> The InQuery (CORI) retrieval model
<li> Cosine similarity model
</ol>

<p>
The parameter to select the model is <tt>retModel</tt>. Valid values are:
<ul> 
<li> <tt>tfidf</tt> or 0 for TFIDF
<li> <tt>okapi</tt> or 1 for Okapi
<li> <tt>kl</tt> or 2 for Simple KL
<li> <tt>inquery</tt> or 3 for INQUERY
<li> <tt>cori_cs</tt> or 4 for CORI_CS
<li> <tt>cos</tt> or 5 for cosine similarity
</ul> 

<p>It is suspected that there is a bug in the implementation of the feedback
for Okapi BM25 retrieval function, because the performance is not as
expected.  

<p>Other common parameters (for all retrieval methods)  are:

<ol>

<li> <tt>index</tt>: The complete name of the index table-of-content file for the database index.

<li> <tt>textQuerySet</tt>: the query text stream 

<li> <tt>resultFile</tt>: the result file

<li> <tt>resultFormat</tt>: whether the result format should be of the 
TREC format (i.e., six-column) or just a simple three-column format 
<tt>&lt;queryID, docID, score&gt;</tt>. String value, either <tt>trec</tt>
for TREC format or <tt>3col</tt> for three column format. The integer
values, zero for non-TREC format, and non-zero for TREC format used in
previous versions of lemur are accepted. Default: TREC format.

<li> <tt>resultCount</tt>: the number of documents to return as result
for each query

<li><tt>feedbackDocuments </tt>: the file of feedback documents to be
used for feedback. In the case of pseudo feedback, this can be a result
file generated from an initial retrieval process. In the case of
relevance feedback, this is usually a 3-column relevance judgment
file. Note that this means you can <em>NOT</em> use a TREC-style
judgment file directly; you must remove the second column to convert it
to three-column.

<li> <tt>feedbackDocCount</tt>: the number of docs to use for feedback
(negative value means using all judged documents for feedback). The
documents in the <tt> feedbackDocuments</tt> are sorted in decreasing
order according to the numerical value in the third column, and then the
top documents are used for feedback.

<li> <tt>feedbackTermCount</tt>: the number of terms to add to a query
    when doing feedback. Note that in the KL-div. approach, the actual
    number of terms is also affected by two other parameters.(See
    below.)
</ol>

<P>
Model-specific parameters are:
<ul>
<LI>For TFIDF:
<ol>

<li> <tt>feedbackPosCoeff</tt>: the coefficient for positive terms in
(positive) Rocchio feedback. We only implemented the positive part and
non-relevant documents are ignored.

<li> <tt>doc.tfMethod</tt>: document term TF weighting method: 0 for
<tt>RawTF</tt>, 1 for <tt>log-TF</tt>, and 2 for <tt>BM25TF</tt>

<li> <tt>doc.bm25K1</tt>: BM25 k1 for doc term TF

<li> <tt>doc.bm25B</tt> : BM25 b for doc term TF

<li> <tt>query.tfMethod</tt>: query term TF weighting method: 0 for
<tt>RawTF</tt>, 1 for <tt>log-TF</tt>, and 2 for <tt>BM25TF</tt>

<li><tt>query.bm25K1</tt>: BM25 k1 for query term TF. bm25B is set to
zero for query terms

</ol>
<li>For Okapi:
<ol>
<li> <tt>BM25K1</tt> : BM25 K1

<li> <tt>BM25B</tt> : BM25 B

<li> <tt>BM25K3</tt>: BM25 K3 

<li> <tt>BM25QTF</tt>: The TF for expanded terms in feedback (the
original paper about the Okapi system is not clear about how this is
set, so it's implemented as a parameter.)

</ol>

<li>For INQUERY:
<ol>
<li><tt>TF_factor</tt>
<li><tt>TF_baseline</tt>

<li> <tt>collCounts</tt> - Use value "USE_INDEX_COUNTS" to use counts
from the index if no separate collection counts file is available.  For
collection selection indexes built from collSell application, that file
is auto generated.
</ol>

<li>For COS:
<ol>
<li> <tt>feedbackPosCoeff</tt>: the coefficient for positive terms in
(positive) Rocchio feedback as for the TFIDF model. 
<li> <tt>L2File</tt> - File containing precomputed L2 Norms (generated
with GenL2Norm). 
</ol>

<li>For KL-divergence:
<ol>

<li> <tt>smoothSupportFile</tt>: The name of the smoothing support file
(e.g., one generated by GenerateSmoothSupport).

<li> <tt>smoothMethod</tt>: One of the four: 
<ul>
<li><tt>jelinikmercer</tt> or <tt>jm</tt> or 0 for Jelinek-Mercer
<li><tt>dirichletprior</tt> or <tt>dir</tt> or 1 for Dirichlet prior 
<li><tt>absolutediscount</tt> or <tt>ad</tt> or 2 for Absolute
discounting
<li><tt>twostage</tt> or <tt>2s</tt> or 3 for two stage.
</ul>

<li> <tt>smoothStrategy</tt>: Either <tt>interpolate</tt> or 0 for
interpolate or <tt>backoff</tt> or 1 for backoff.

<li> <tt>adjustedScoreMethod</tt>: Which type of score to output, one of:
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
<li><tt>mixture</tt> or <tt>mix</tt> or 0 for mixture.
<li><tt>divmin</tt> or <tt>div</tt> or 1 for div min
<li><tt>markovchain</tt> or <tt>mc</tt> or 2 for markov chain
<li><tt>relevancemodel1</tt> or <tt>rm1</tt> or 3 for relevance model 1.
<li><tt>relevancemodel2</tt> or <tt>rm2</tt> or 4 for relevance model 2.
</ul>
</ol>
<li>  Method-specific feedback parameters:

<p> For all interpolation-based approaches (i.e., the new query model is
an interpolation of the original model with a (feedback) model computed
based on the feedback documents), the following four parameters apply:

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
</ul>

Parameters <tt>feedbackTermCount</tt>, <tt>feedbackProbThresh</tt>, and
<tt>feedbackProbSumThresh</tt> work conjunctively to control the
truncation, i.e., the truncated model must satisfy all the three
constraints.

<p> All the three feedback methods also recognize the parameter
<tt>feedbackMixtureNoise</tt> (default value :0.5), but with 
<em>different</em> interpretations.

<ul>

<li> For the collection mixture model method,
<tt>feedbackMixtureNoise</tt> is the collection model selection
probability in the mixture model. That is, with this probability, a word
is picked according to the collection language model, when a feedback
document is "generated".

<li> For the divergence minimization method,
<tt>feedbackMixtureNoise</tt> means the weight of the divergence from
the collection language model. (The higher it is, the farther the
estimated model is from the collection model.)

<li> For the Markov chain method, <tt>feedbackMixtureNoise</tt> is the
probability of <em>not</em> stopping, i.e., <tt>1- alpha</tt>, where
alpha is the stopping probability while walking through the chain.

</ul>

<p> In addition, the collection mixture model also recognizes the
parameter <tt>emIterations</tt>, which is the maximum number of
iterations the EM algorithm will run. Default: 50. (The EM algorithm can
terminate earlier if the log-likelihood converges quickly, where
convergence is measured by some hard-coded criterion. See the source
code in <tt>SimpleKLRetMethod.cpp</tt> for details. )
 */

#include "common_headers.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include "RetMethodManager.hpp"
#include "ResultFile.hpp"

using namespace lemur::api;

namespace LocalParameter {
  std::string feedbackDocuments;
  void get() {
    // judgments file name
    feedbackDocuments = ParamGetString("feedbackDocuments",""); 
  }
};

void GetAppParam()
{
  RetrievalParameter::get();
  SimpleKLParameter::get();
  LocalParameter::get();
}




void process(QueryRep *qr, const string& qid, ResultFile *judgments, 
	     RetrievalMethod *method, ResultFile *docPool, 
	     IndexedRealVector &results, ResultFile &resFile)
{
  // do relevance feedback
  
  PseudoFBDocs *topDoc;
  bool ignoreWeights = ! (RetrievalParameter::fbDocCount > 0 &&
			  RetrievalParameter::retModel == "kl" &&
			  (SimpleKLParameter::qryPrm.fbMethod == 
			   SimpleKLParameter::RM1 || 
			   SimpleKLParameter::qryPrm.fbMethod == 
			   SimpleKLParameter::RM2));

  IndexedRealVector *judgmentEntries;  
  if (!judgments->findResult(qid,judgmentEntries)) {
    cerr << "Warning: judgments not found for query "<< qid << endl;
    return;
  }
  
  topDoc = new PseudoFBDocs(*judgmentEntries, RetrievalParameter::fbDocCount,
			    ignoreWeights);
  // -1 means using all docs
  method->updateQuery(*qr, *topDoc);

  delete topDoc;

  PseudoFBDocs *workSet;

  IndexedRealVector workSetRes;

  if (RetrievalParameter::useWorkingSet) {
    docPool->getResult(qid, workSetRes);
    workSet = new PseudoFBDocs(workSetRes, -1); // -1 means using all docs
    method->scoreDocSet(*qr,*workSet,results);
    delete workSet;
  } else {
    method->scoreCollection(*qr, results);
  }
  results.Sort();
  resFile.writeResults(qid, &results, RetrievalParameter::resultCount);
}


int AppMain(int argc, char *argv[]) {

  Index  *ind;

  try {
    ind  = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  } 
  catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("RelFBEval", "Can't open index, check parameter index");
  }

  ofstream result(RetrievalParameter::resultFile.c_str());
  ResultFile resFile(RetrievalParameter::TRECresultFileFormat);
  resFile.openForWrite(result, *ind);

  ifstream *workSetStr;
  ResultFile *docPool;
  if (RetrievalParameter::useWorkingSet) {
    workSetStr = new ifstream(RetrievalParameter::workSetFile.c_str(), ios::in);
    if (workSetStr->fail()) {
      throw Exception("RelFBEval", "can't open working set file");
    }
    docPool = new ResultFile(false); // working set is always simple format
    docPool->openForRead(*workSetStr, *ind);
  }



  ifstream *judgmentStr;
  ResultFile *judgments;
  judgmentStr = new ifstream(LocalParameter::feedbackDocuments.c_str(), ios::in);
  if (judgmentStr->fail()) {
      throw Exception("RelFBEval", "can't open judgment file");
  }
  judgments = new ResultFile(false); // judgment file is always simple format
  judgments->load(*judgmentStr, *ind);

  lemur::retrieval::ArrayAccumulator accumulator(ind->docCount());

  IndexedRealVector results(ind->docCount());

  RetrievalMethod *model = NULL;
  model = RetMethodManager::createModel(ind, &accumulator, 
					RetrievalParameter::retModel);

  DocStream *qryStream;
  try {
    qryStream = new lemur::parse::BasicDocStream(RetrievalParameter::textQuerySet);
  } 
  catch (Exception &ex) {
    ex.writeMessage(cerr);
    throw Exception("RelFBEval", 
		    "Can't open query file, check parameter textQuery");
  }

  qryStream->startDocIteration();
  TextQuery *q;
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : "<< q->id() << endl;
    QueryRep * qr = model->computeQueryRep(*q);
    process(qr, q->id(), judgments, model, docPool, results, resFile);
    delete qr;
    delete q;
  }

  delete judgments;
  delete judgmentStr;
  if (RetrievalParameter::useWorkingSet) {
    delete docPool;
    delete workSetStr;
  }
  delete model;
  result.close();
  delete qryStream;
  delete ind;
  return 0;
}

