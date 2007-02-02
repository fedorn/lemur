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

#include "DistSearchMethod.hpp"
using namespace lemur::api;

void lemur::distrib::DistSearchMethod::scoreIndexSet(const Query &qry, 
                                                     const IndexedRealVector &indexset, 
                                                     DocScoreVector** results) {

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

void lemur::distrib::DistSearchMethod::scoreIndexSet(const Query &qry, 
                                                     const vector<string> &indexset, 
                                                     DocScoreVector** results) {

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


void lemur::distrib::DistSearchMethod::indexToID(const Index* ind, 
                                                 const IndexedRealVector* ivec, 
                                                 DocScoreVector* dvec) {

  IndexedRealVector::const_iterator i;
  for (i=ivec->begin();i!= ivec->end(); i++) {
    dvec->PushValue(ind->document((*i).ind), (*i).val);
  }
}

/**********************  PRIVATE METHODS ****************************/
void lemur::distrib::DistSearchMethod::doSingleRetr() {
  // open the individual db
  Index* single = IndexManager::openIndex(ParamGetString("index"));
  int numdocs = single->docCount();
  lemur::retrieval::ArrayAccumulator* accum = new lemur::retrieval::ArrayAccumulator(numdocs);
  // create the model according to parameter
  std::string ret = ParamGetString("retModel", "inquery");
  //  RetrievalMethod* model = RetMethodManager::createModel(single, accum, defrt);
  RetrievalMethod* model = RetMethodManager::createModel(single, accum, ret);
  IndexedRealVector* res = new IndexedRealVector(numdocs);
  QueryRep* qr = model->computeQueryRep(*query);
  model->scoreCollection(*qr, *res);
  DocScoreVector* dscore = new DocScoreVector();
  res->Sort();
  if (returnCount<res->size()){
    res->resize(returnCount);
  }
  // convert results from internal to external docids
  indexToID(single, res, dscore);
  //  dscore->Sort();
  
  allres[reslen] = dscore;
  reslen++;
  delete (qr);
  delete res;
  delete accum;
  delete model;
  delete single;
}
