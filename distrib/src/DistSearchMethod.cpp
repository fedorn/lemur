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

#include "DistSearchMethod.hpp"

void DistSearchMethod::scoreIndexSet(Query &qry, IndexedRealVector &indexset, DocScoreVector** results) {

  query = &qry;
  allres = results;
  reslen = 0;

  for (int i=0;i<indexset.size();i++) {
    // get the name for the parameter file for the individual index
    // and push its parameters onto the stack
    ParamPushFile(ParamGetString(csindex->document(indexset[i].ind)));
    doSingleRetr();
    // we're done with these parameters.  get rid of them to prepare for next
    ParamPopFile();
  }
}

void DistSearchMethod::scoreIndexSet(Query &qry, vector<string> &indexset, DocScoreVector** results) {

  query = &qry;
  allres = results;
  reslen = 0;

  for (int i=0;i<indexset.size();i++) {
    // get the name for the parameter file for the individual index
    // and push its parameters onto the stack
    ParamPushFile(ParamGetString(indexset[i]));
    doSingleRetr();
    // we're done with these parameters.  get rid of them to prepare for next
    ParamPopFile();
  }
}

void DistSearchMethod::indexToID(Index* ind, IndexedRealVector* ivec, DocScoreVector* dvec) {

  IndexedRealVector::iterator i;
  for (i=ivec->begin();i!= ivec->end(); i++) {
    dvec->PushValue(ind->document((*i).ind), (*i).val);
  }
}

/**********************  PRIVATE METHODS ****************************/
void DistSearchMethod::doSingleRetr() {
  // open the individual db
  Index* single = IndexManager::openIndex(ParamGetString("index"));
  int numdocs = single->docCount();
  ArrayAccumulator* accum = new ArrayAccumulator(numdocs);
  // create the model according to parameter
  RetrievalMethod* model = RetMethodManager::createModel(single, accum, defrt);
  IndexedRealVector* res = new IndexedRealVector(numdocs);
  QueryRep* qr = model->computeQueryRep(*query);
  model->scoreCollection(*qr, *res);
  DocScoreVector* dscore = new DocScoreVector();
  //  dscore->clear();

  // convert results from internal to external docids
  indexToID(single, res, dscore);
  //  dscore->Sort();
  
  allres[reslen] = dscore;
  reslen++;

  delete res;
  delete accum;
  delete model;
  delete single;
}



