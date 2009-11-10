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

/*! \page StructuredQueryEvaluation Structured Query Evaluation

<p> This application runs retrieval experiments to evaluate the
performance of the structured query model using the inquery retrieval
method. StructQueryEval requires that its index parameter be a
positional index (KeyfileIncIndex).

<p>Feedback is implemented as a WSUM of the original query combined with
terms selected from the feedback documents based on belief score. The
expanded query has the form:

<p>
<pre>
#wsum( (1 - a) &lt;original query&gt;
      a*w1  t1
      a*w2  t2
      ...
      a*wN  tN
      )

</pre><br>
where <b>a</b> is the value of the parameter <tt>feedbackPosCoeff</tt>.

<p>
Scoring is either done over a working set of documents (essentially
re-ranking), or over the whole collection. This is indicated by the
parameter "useWorkingSet". When "useWorkingSet" has either a non-zero
(integer) value or the value <tt>true</tt>, scoring will be on a working set
specified in a file given by "workingSetFile". The file should have three
columns. The first is the query id; the second the document id; and the
last a numerical value, which is ignored. By default, scoring is on the
whole collection.

<p>
The parameters are:
<p>
<ol>

<li> <tt>index</tt>: The complete name of the index table-of-content file 
for the database index. This must be a positional index (currently KeyfileIncIndex).

<li> <tt>textQuery</tt>: the query text stream parsed by ParseInQuery

<li> <tt>resultFile</tt>: the result file

<li> <tt>resultFormat</tt>: whether the result format should be of the 
TREC format (i.e., six-column) or just a simple three-column format 
<tt>&lt;queryID, docID, score&gt</tt>. String value, either <tt>trec</tt>
for TREC format or <tt>3col</tt> for three column format. The integer
values, zero for non-TREC format, and non-zero for TREC format used in
previous versions of lemur are accepted. Default: TREC format.
<li> <tt>resultCount</tt>: the number of documents to return as result 
for each query
<li> <tt>defaultBelief</tt>: The default belief for a document: Default=0.4
<li> <tt>feedbackDocCount</tt>: the number of docs to use for pseudo-feedback
(0 means no-feedback)

<li> <tt>feedbackTermCount</tt>: the number of terms to add to a query 
when doing feedback.
<li> <tt>feedbackPosCoeff</tt>: the coefficient for positive terms in 
the expanded query.
</ol>
 */

#include "common_headers.hpp"
#include "IndexManager.hpp"
#include "RetParamManager.hpp"
#include "ResultFile.hpp"
#include "BasicDocStream.hpp"

using namespace lemur::api;


void GetAppParam() {
  RetrievalParameter::get();
  InQueryParameter::get();
}

/// A retrieval evaluation program
int AppMain(int argc, char *argv[]) {
  Index  *ind;
  try {
    ind  = IndexManager::openIndex(RetrievalParameter::databaseIndex);
    // Test that index returns InvFPDocList * from docInfoList.
    lemur::index::InvFPDocList *dList;
    DocInfoList *dl;
    // use last term in vocab to make this a small read (hopefully).
    COUNT_T lastTerm = ind->termCountUnique();
    dl = ind->docInfoList(lastTerm);
    dList = dynamic_cast<lemur::index::InvFPDocList *>(dl);
    if (dList == NULL) {
      cerr << "WARNING: StructQueryEval: InvFPDocList required "
           << "from index for\nproximity operators. " 
           << "Setting all positions to 0." << endl;

    }
    delete(dl);
  } catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("StructQueryEval", 
                    "Can't open index, check parameter index");
  }

  DocStream *qryStream;
  try {
    qryStream = new lemur::parse::BasicDocStream(RetrievalParameter::textQuerySet);
  } catch (Exception &ex) {
    ex.writeMessage(cerr);
    throw Exception("StructQueryEval", 
                    "Can't open query file, check parameter textQuery");
  }
  ofstream result(RetrievalParameter::resultFile.c_str());
  ResultFile resFile(RetrievalParameter::TRECresultFileFormat);
  resFile.openForWrite(result, *ind);
  ifstream *workSetStr;
  ResultFile *docPool;
  if (RetrievalParameter::useWorkingSet) {
    workSetStr = new ifstream(RetrievalParameter::workSetFile.c_str(), ios::in | ios::binary);
    if (workSetStr->fail()) {
      throw Exception("StructQueryEval", "can't open working set file");
    }
    docPool = new ResultFile(false); // working set is always simple format
    docPool->openForRead(*workSetStr, *ind);
  }
  IndexedRealVector results(ind->docCount());
  lemur::retrieval::InQueryRetMethod *model = 
    new lemur::retrieval::InQueryRetMethod(*ind, 
                                           InQueryParameter::defaultBelief,
                                           InQueryParameter::fbTermCount,
                                           InQueryParameter::fbCoeff,
                                           InQueryParameter::cacheIDF);
  qryStream->startDocIteration();
  TextQuery *q;
  IndexedRealVector workSetRes;
  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : "<< q->id() << endl;
    QueryRep *qr = model->computeQueryRep(*q);
    PseudoFBDocs *workSet;
    if (RetrievalParameter::useWorkingSet) {
      docPool->getResult(q->id(), workSetRes);
      workSet = new PseudoFBDocs(workSetRes, -1); // -1 means using all docs
      model->scoreDocSet(*qr, *workSet, results);
    } else {
      model->scoreCollection(*qr, results);
    }
    results.Sort();
    if (RetrievalParameter::fbDocCount > 0) {
      PseudoFBDocs *topDoc = new PseudoFBDocs(results, 
                                              RetrievalParameter::fbDocCount);
      model->updateQuery(*qr, *topDoc);
      if (RetrievalParameter::useWorkingSet) {
        model->scoreDocSet(*qr, *workSet, results);
      } else {
        model->scoreCollection(*qr, results);
      } 
      results.Sort();
      delete topDoc;
    }
    resFile.writeResults(q->id(), &results, RetrievalParameter::resultCount);
    if (RetrievalParameter::useWorkingSet) {
      delete workSet;
    }
    delete qr;
    delete q;
  }
  result.close();
  if (RetrievalParameter::useWorkingSet) {
    delete docPool;
    delete workSetStr;
  }
  delete model;
  delete qryStream;
  delete ind;
  return 0;
}
