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


/** This is a sample application that does distributed retrieval, using
    a collection selection index and individual indexes.  Database ranking
    is done using the CORIRetMethod (the only db ranking implemented thus
    far).  Database score merging uses CORIMergeMethod (the only merge 
    method implemented so far).

    Parameters should be set as follows:
    index = the collection selection database
    collCounts = collection counts file for the csindex (needed by CORI)
    ranksFile = file to write rankings results to (optional)
    resultFile = file to write final results
    resultCount = maximum number of results to output for each query (default to 1000)
    textQuery = file of text queries in docstream format
    cutoff = maximum number of databases to search (default to 10)
    "dbids" = "db's param file" - required for each database in the collection selection index.  key should be the dbcharid and value should be name of file that has parameters for that database: 
            index = the individual database
	    retModel = the retrieval model to use
	    "modelvals" - whatever parameters are required for that retModel
    
**/

#include "CORIRetMethod.hpp"
#include "DistSearchMethod.hpp"
#include "CORIMergeMethod.hpp"
#include "Param.hpp"
#include "RetParamManager.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"

namespace LocalParameter {
  // the index cutoff for how many indexes to search
  static int cutoff;
  static string ranksfile;
  static void get() {
    cutoff = ParamGetInt("cutoff", 10); // default is 10
    ranksfile = ParamGetString("ranksFile", ""); // write out ranking scores
  }
};

void GetAppParam() {
  RetrievalParameter::get();
  CORIParameter::get();
  LocalParameter::get();
}

int AppMain(int argc, char *argv[]) {
  // this should be a collection selection index
  Index *csindex;
  try {
    csindex = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  }
  catch (Exception &ex) {
    ex.writeMessage(cerr);
    throw Exception("DistRetEval", "Can't open index, check parameter index");
  }
  
  DocStream *qryStream;
  try {
    qryStream = new BasicDocStream(RetrievalParameter::textQuerySet);
  }
  catch (Exception &ex){
    ex.writeMessage(cerr);
    throw Exception("DistRetEval", "Can't open query file, check parameter textQuery");
  }

  int i;
  int doccount = csindex->docCount();
  ArrayAccumulator accum(csindex->docCount());
  IndexedRealVector rankings(doccount);
  CORIRetMethod model(*csindex, accum, CORIParameter::collectionCounts);
  DistSearchMethod search(csindex);
  DocScoreVector** scoreset = new DocScoreVector*[LocalParameter::cutoff];
  CORIMergeMethod merger;
  DocScoreVector results;

  ofstream resfile(RetrievalParameter::resultFile);
  ofstream writeranks;
  if (LocalParameter::ranksfile.compare("") != 0)
    writeranks.open(LocalParameter::ranksfile.c_str());

  qryStream->startDocIteration();
  TextQuery *q;

  while (qryStream->hasMore()) {
    Document *d = qryStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : " << q->id() << endl;
    QueryRep *qr = model.computeQueryRep(*q);
    model.scoreCollection(*qr, rankings);

    rankings.Sort();

    // write out the rankings for all databases if there was a file specified
    if (writeranks.is_open()) {
      for (i=0;i<doccount;i++) {
	writeranks << q->id() << " Q0 " << csindex->document(rankings[i].ind) << " 0 " << rankings[i].val << " Exp" << endl;
      }
    }

    // we want to search only so many databases
    rankings.assign(rankings.begin(), rankings.begin()+LocalParameter::cutoff);
    // search those databases and put the scores into scoreset
    search.scoreIndexSet(*q, rankings, scoreset);
    // now merge the scores
    merger.mergeScoreSet(rankings, scoreset, results);

    for (i=0;i<RetrievalParameter::resultCount && i<results.size();i++) 
      resfile << q->id() << " Q0 " << results[i].id << " 0 " << results[i].val << " Exp" << endl;

    rankings.clear();
    results.clear();
    for (i=0;i<LocalParameter::cutoff;i++) 
	delete (scoreset[i]);
    delete q;
    delete qr;
  }
  resfile.close();
  if (writeranks.is_open())
    writeranks.close();
  delete csindex;
  delete qryStream;
  delete[]scoreset;

  return 0;
}
