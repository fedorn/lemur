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


/// Evaluating a query model using the KL-divergence retrieval function



/*! \page QueryModelEval  Evaluating an (expanded) query model with the KL-divergence retrieval model

This application loads an expanded query model and evaluates it with
the KL-divergence retrieval model.

Parameters:

1) "index": the index table-of-content file for the database index. It should have a file extension with exactly three characters. The convention assumed is
".lmr" or ".LMR" means the basic indexer and ".toc" or ".TOC" means the InvFP indexer. (These extensions should be modified to reflect the characteristics of the corresponding indexer, but this has not been done yet.)

2) "queryModelFile": the file of the expanded query model 

3) "resultFile": the result file 

4) "resultCount": the number of documents to return for each query

4) "feedbackDocCount": the number of docs to use for pseudo-feedback (0 means no-feedback)

5) "feedbackTermCount": the number of terms to add to a query when doing feedback

6) "expandedQuery": the expanded query file

7) "feedbackMethod": feedback method (0, 1, 2, 3 for maximum-likelihood, mixture model, divergence minimization, and Markov chain respectively).


8) other feedback-method-specific parameters 


 */


#include "Param.hpp"
#include "IndexManager.hpp"
#include "BasicDocStream.hpp"
#include <iostream.h>
#include "SimpleKLRetMethod.hpp"
#include "RetrievalEngine.hpp"
#include "ParamManager.hpp"
#include "ResultFile.hpp"
namespace LocalParameter {
  String queryModel;
  bool TRECResultFormat;

  void get() {
    queryModel = ParamGetString("queryModel","");
    TRECResultFormat = ParamGetInt("resultFormat",1); // default is TREC format
 
  }
};

void GetAppParam()
{
  LocalParameter::get();
  RetrievalParameter::get();
  SimpleKLParameter::get();
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


/// A query model estimation program
int AppMain(int argc, char *argv[]) {
  
  Index  *ind = IndexManager::openIndex(RetrievalParameter::databaseIndex);
  ifstream qmodel(LocalParameter::queryModel, ios::in);
  if (qmodel.fail()) {
    throw Exception("AppMain", "can't open the query model file, check the value for parameter queryModelFile");
  }
  
  ofstream result(RetrievalParameter::resultFile);

  ResultFile resFile(LocalParameter::TRECResultFormat);

  resFile.openForWrite(result, *ind);

  SimpleKLRetMethod model(*ind, SimpleKLParameter::smoothSupportFile);
  
  model.setDocSmoothParam(SimpleKLParameter::docPrm);
  model.setQueryModelParam(SimpleKLParameter::qryPrm);

  RetrievalEngine eng(model);

  IndexedRealVector res;
  char qid[300];

  SimpleKLQueryModel *q;
  while (qmodel >> qid) {
    cout << "Query "<< qid << endl;
    q = new SimpleKLQueryModel(*ind);
    q->load(qmodel);
    eng.scoreInvertedIndex(*q,res);
    resFile.writeResults(qid, &res, RetrievalParameter::resultCount);
    delete q;
  }
  
  result.close();

  delete ind;
  return 0;
}


