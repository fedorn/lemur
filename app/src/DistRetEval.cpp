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


/** author: tnt, lsi */

/*! \page DistRetEval Distributed Retrieval
    This is a sample application that does distributed retrieval, using a
    resource selection index and individual indexes. resource selection is done
    using the CORI_CS  (the only resource selection implemented thus far).
    results merging uses either CORIMergeMethod, SingleRegrMergeMethod, or MultiRegrMergeMethod.
    (If using CORIMerge, should use INQUERY for the retrieval method of each 
    individual database.)

    Parameters should be set as follows:
    index = the collection selection database
    collCounts = collection counts file for the csindex (needed by CORI)
    ranksFile = file to write rankings results to (optional)
    resultFile = file to write final results
    resultCount = maximum number of results to output for each query (default to 1000)
    textQuery = file of text queries in docstream format
    cutoff = maximum number of databases to search (default to 10)
    "dbids" = "db's param file" - required for each database in the collection selection index.  key should be the db's string id as it was named/indexed in the collection selection index,  and value should be name of file that has parameters for that database: 
            index = the individual database
            retModel = the retrieval model to use
            mergeMethod = the merging method to use
            "modelvals" - whatever parameters are required for that retModel
    CSTF_factor  The TFfactor parameter in the CORI_CS resource
    selection method.

    CSTF_baseline The TFbaseline parameter in the CORI_CS resource
    selection method.

    mergeMethod  resource merging method (0 for CORI results merging
    method, 1 for single regression results merging method, 2 for multiple regression merge)

    Merginge Method-specific parameters:

         For CORI merging Method: None
    
         For  Single and Multiple regression merging methods:
    
         1. csDbDataBaseIndex the centralized sampling database index

         2. DOCTF_factor  The TFfactor parameter in the INQUERY retrieval
         method for the centralized sampling database.
         
         3. DOCTF_baseline The TFbaseline parameter in the INQUERY retrieval method for the centralized sampling database.


**/

#include "CORIRetMethod.hpp"
#include "DistSearchMethod.hpp"
#include "DistMergeMethod.hpp"
#include "CORIMergeMethod.hpp"
#include "SingleRegrMergeMethod.hpp"
#include "MultiRegrMergeMethod.hpp"
#include "Param.hpp"
#include "RetParamManager.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"

using namespace lemur::api;
using namespace lemur::distrib;

namespace LocalParameter {
  // the index cutoff for how many indexes to search
  static int cutoff;
  static string ranksfile;
  //  static char* textQuerySet2;
  static void get() {
    cutoff = ParamGetInt("cutoff", 10); // default is 10
    ranksfile = ParamGetString("ranksFile", ""); // write out ranking scores
    //    textQuerySet2=strdup(ParamGetString("textQuerySet2",""));
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
  lemur::retrieval::ArrayAccumulator *accumCsDb;
  IndexedRealVector *rankingsCsDb;
  lemur::retrieval::CORIRetMethod *modelCsDb;
  try {
    csindex = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  }
  catch (Exception &ex) {
    ex.writeMessage(cerr);
    throw Exception("DistRetEval", "Can't open the collection selection index, check parameter index");
  }


  if ( DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE){
    //use single type regression method
    try {
      SingleRegrMergeMethodParameter::get();
      csdbindex = IndexManager::openIndex(SingleRegrMergeMethodParameter::csDbDataBaseIndex);
      int doccount = csdbindex->docCount();
      accumCsDb=new lemur::retrieval::ArrayAccumulator (doccount);
      rankingsCsDb=new IndexedRealVector(doccount);
      modelCsDb=new  lemur::retrieval::CORIRetMethod (*csdbindex,*accumCsDb,"USE_INDEX_COUNTS",0);

    }
    catch (Exception &ex) {
      ex.writeMessage(cerr);
      throw Exception("DistRetEval", "Can't open the centralized database index, check parameter index");
    }
  }

  if ( DistMergeMethodParameter::mergeMethod==MULTITYPEREGR_MERGE){
    //use multiple type regression method
    try {
      MultiRegrMergeMethodParameter::get();
      csdbindex = IndexManager::openIndex(MultiRegrMergeMethodParameter::csDbDataBaseIndex);
      int doccount = csdbindex->docCount();
      accumCsDb=new lemur::retrieval::ArrayAccumulator (doccount);
      rankingsCsDb=new IndexedRealVector(doccount);
      modelCsDb=new lemur::retrieval::CORIRetMethod (*csdbindex,*accumCsDb,"USE_INDEX_COUNTS",0);

    }
    catch (Exception &ex) {
      ex.writeMessage(cerr);
      throw Exception("DistRetEval", "Can't open the centralized database index, check parameter index");
    }
  }
  
  DocStream *qryStream;


  try {
    qryStream = new lemur::parse::BasicDocStream(RetrievalParameter::textQuerySet);
  }
  catch (Exception &ex){
    ex.writeMessage(cerr);
    throw Exception("DistRetEval", "Can't open query file, check parameter textQuery");
  }

  int i;
 
  //create the collection selection retrieval model
  int doccount = csindex->docCount();
  lemur::retrieval::ArrayAccumulator accum(csindex->docCount());
  IndexedRealVector rankings(doccount);
  lemur::retrieval::CORIRetMethod model(*csindex, accum, CORIParameter::collectionCounts,1);
  lemur::distrib::DocScoreVector results;

  //Notice assume that collectionCounts i USE_INDEX_COUNT
  lemur::distrib::DistSearchMethod search(csindex,RetMethodManager::INQUERY);
  lemur::distrib::DocScoreVector** scoreset = new lemur::distrib::DocScoreVector*[LocalParameter::cutoff];
  lemur::distrib::DocScoreVector resultsCsDb;

  cout<<"Write Results File to "<<RetrievalParameter::resultFile<<endl;
  ofstream resfile(RetrievalParameter::resultFile);
  assert(resfile);
  ofstream writeranks;
  if (LocalParameter::ranksfile.compare("") != 0)
    writeranks.open(LocalParameter::ranksfile.c_str());

  qryStream->startDocIteration();
  TextQuery *q;
  DistMergeMethodParameter::get();

  while (qryStream->hasMore() ) {
    Document *d = qryStream->nextDoc();
    //Document *dCs=qryCSStream->nextDoc();
    q = new TextQuery(*d);
    cout << "query : " << q->id() << endl;

    //to calculate the centralized scores; 
    if (DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE ||
        DistMergeMethodParameter::mergeMethod==MULTITYPEREGR_MERGE){
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
    if(rankings.size() == 0) {
      cerr << "No database selected for query " << q->id() << endl;
      // clean up.
      delete q;
      delete qr;
      continue;
    }

    rankings.Sort();

    // write out the rankings for all databases if there was a file specified
    if (writeranks.is_open()) {
      for (i=0;i<rankings.size();i++) {
        writeranks << q->id() << " Q0 " << csindex->document(rankings[i].ind) << " 0 " << rankings[i].val << " Exp" << endl;
      }
    }

    // we want to search only so many databases
   if(rankings.size()>LocalParameter::cutoff)
     rankings.resize(LocalParameter::cutoff);
    // set return document counts for each individual databases
    search.setReturnCount(RetrievalParameter::resultCount);
    // search those databases and put the scores into scoreset
    search.scoreIndexSet(*q, rankings, scoreset);

    // now merge the scores
    if (DistMergeMethodParameter::mergeMethod==CORI_MERGE){
      lemur::distrib::CORIMergeMethod merger;
      merger.mergeScoreSet(rankings, scoreset, results);
    }else if  (DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE){
      lemur::distrib::SingleRegrMergeMethod singleRegrMerger;
      singleRegrMerger.calcRegrParams(rankings,&resultsCsDb,scoreset);
      singleRegrMerger.mergeScoreSet(rankings, scoreset, results);
    }else if  (DistMergeMethodParameter::mergeMethod==MULTITYPEREGR_MERGE){
      lemur::distrib::MultiRegrMergeMethod multiRegrMerger;
      multiRegrMerger.calcRegrParams(rankings,&resultsCsDb,scoreset);
      multiRegrMerger.mergeScoreSet(rankings, scoreset, results);
    }

    for (i=0;i<RetrievalParameter::resultCount && i<results.size();i++) 
      resfile << q->id() << " Q0 " << results[i].id << " 0 " << results[i].val << " Exp" << endl;

    //    for (i=0;i<LocalParameter::cutoff;i++) 
    for (i=0;i<rankings.size();i++) 
        delete (scoreset[i]);

    rankings.clear();
    results.clear();
    resultsCsDb.clear();

    delete q;
    delete qr;
  }
  resfile.close();
  if (writeranks.is_open())
    writeranks.close();
  delete csindex;
  delete qryStream;
  delete[]scoreset;

  if ( DistMergeMethodParameter::mergeMethod==SINGLETYPEREGR_MERGE ||
       DistMergeMethodParameter::mergeMethod==MULTITYPEREGR_MERGE){
    delete accumCsDb;
    delete rankingsCsDb;
    delete modelCsDb;
    delete csdbindex;
  }

  return 0;
}
