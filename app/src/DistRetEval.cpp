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


/*
 * NAME DATE - COMMENTS
 * tnt 06/2002 - created
 * lsi 09/2002 - modified to support other merge methods
 *
 *========================================================================*/

/** This is a sample application that does distributed retrieval, using
    a collection selection index and individual indexes.  Database ranking
    is done using the CORIRetMethod (the only db ranking implemented thus
    far).  Merging can be done using any available DistMergeMethod.

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
	    mergeMethod = the merging method to use
	    "modelvals" - whatever parameters are required for that retModel
    
**/

#include "CORIRetMethod.hpp"
#include "DistSearchMethod.hpp"
#include "DistMergeMethod.hpp"
#include "CORIMergeMethod.hpp"
#include "SingleRegrMergeMethod.hpp"
#include "Param.hpp"
#include "RetParamManager.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"

namespace LocalParameter {
  // the index cutoff for how many indexes to search
  static int cutoff;
  static string ranksfile;
  static char* textQuerySet2;
  static void get() {
    cutoff = ParamGetInt("cutoff", 10); // default is 10
    ranksfile = ParamGetString("ranksFile", ""); // write out ranking scores
    textQuerySet2=strdup(ParamGetString("textQuerySet2",""));
  }
};

void GetAppParam() {
  RetrievalParameter::get();
  CORIParameter::get();
  LocalParameter::get();
  DistMergeMethodParameter::get();
}

int AppMain(int argc, char *argv[]) {
  // this should be a collection selection index
  Index *csindex;
  // the centralized sample database
  Index *csdbindex;
  int csdoccount                      ;
  ArrayAccumulator *accumCsDb;
  IndexedRealVector *rankingsCsDb;
  CORIRetMethod *modelCsDb;
  try {
    csindex = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  }
  catch (Exception &ex) {
    ex.writeMessage(cerr);
    throw Exception("DistRetEval", "Can't open the collection selection index, check parameter index");
  }


  if ( DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE){
    //use some regression method
    try {
      SingleRegrMergeMethodParameter::get();
      csdbindex = IndexManager::openIndex(SingleRegrMergeMethodParameter::csDbDataBaseIndex);
      int doccount = csdbindex->docCount();
      accumCsDb=new ArrayAccumulator (doccount);
      rankingsCsDb=new IndexedRealVector(doccount);
      modelCsDb=new   CORIRetMethod (*csdbindex,*accumCsDb,"USE_INDEX_COUNTS",0);

    }
    catch (Exception &ex) {
      ex.writeMessage(cerr);
      throw Exception("DistRetEval", "Can't open the centralized database index, check parameter index");
    }
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
 
  //creat the collecion selection retireval model
  int doccount = csindex->docCount();
  ArrayAccumulator accum(csindex->docCount());
  IndexedRealVector rankings(doccount);
  //creat the collecion selection retireval model
  CORIRetMethod model(*csindex, accum, CORIParameter::collectionCounts,1);
  DocScoreVector results;


  //Notice assume that collectionCounts i USE_INDEX_COUNT


  DistSearchMethod search(csindex,RetMethodManager::CORI_DOC);
  DocScoreVector** scoreset = new DocScoreVector*[LocalParameter::cutoff];
  DocScoreVector resultsCsDb;
  CORIMergeMethod merger;
  SingleRegrMergeMethod singleRegrMerger;



  cout<<"Write Results File to "<<RetrievalParameter::resultFile<<endl;
  ofstream resfile(RetrievalParameter::resultFile);
  assert(resfile);
  ofstream writeranks;
  if (LocalParameter::ranksfile.compare("") != 0)
    writeranks.open(LocalParameter::ranksfile.c_str());

  qryStream->startDocIteration();
  TextQuery *q;
  TextQuery *qCsDb;
  DistMergeMethodParameter::get();

  while (qryStream->hasMore() ) {
    Document *d = qryStream->nextDoc();
    //Document *dCs=qryCSStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : " << q->id() << endl;

    //to calculate the centralized scores;
 
    if (DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE){
      QueryRep *qrCs = modelCsDb->computeQueryRep(*q);
      resultsCsDb.clear();
      modelCsDb->scoreCollection(*qrCs, *rankingsCsDb);    
      for (i=0;i<rankingsCsDb->size();i++) {
	resultsCsDb.PushValue(csdbindex->document((*rankingsCsDb)[i].ind), (*rankingsCsDb)[i].val);
      }
      rankingsCsDb->clear();
      delete qrCs;
    }

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
    // set return document counts for each individual databases
    search.setReturnCount(RetrievalParameter::resultCount);
    // search those databases and put the scores into scoreset
    search.scoreIndexSet(*q, rankings, scoreset);
    // now merge the scores

    if (DistMergeMethodParameter::mergeMethod==CORI_MERGE){
      merger.mergeScoreSet(rankings, scoreset, results);
    }else if  (DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE){
      singleRegrMerger.calcRegrParams(rankings,&resultsCsDb,scoreset);
      singleRegrMerger.mergeScoreSet(rankings, scoreset, results);
    }

    for (i=0;i<RetrievalParameter::resultCount && i<results.size();i++) 
      resfile << q->id() << " Q0 " << results[i].id << " 0 " << results[i].val << " Exp" << endl;

    rankings.clear();
    results.clear();
    resultsCsDb.clear();

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

  if ( DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE){
    delete accumCsDb;
    delete rankingsCsDb;
    delete modelCsDb;
  }

  return 0;
}
