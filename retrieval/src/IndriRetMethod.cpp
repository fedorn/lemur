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

#include "IndriRetMethod.hpp"
#include "indri/QueryEnvironment.hpp"
#include "indri/RMExpander.hpp"

using namespace indri::api;
using namespace lemur::api;

lemur::retrieval::IndriRetMethod::IndriRetMethod(const Index &dbIndex) : 
  RetrievalMethod(dbIndex) {
  // if it isn't an indri repository, die screaming now.
  const lemur::index::LemurIndriIndex *rep = dynamic_cast<const lemur::index::LemurIndriIndex *>(&ind);
  if (rep == NULL) {
    LEMUR_THROW(LEMUR_RUNTIME_ERROR, 
                "IndriRetMethod requires a LemurIndriIndex.");
  }

  env = new QueryEnvironment();
  env->addIndex(rep->getRepositoryName());
}

/// clean up.
lemur::retrieval::IndriRetMethod::~IndriRetMethod() {
  delete(env);
}

void lemur::retrieval::IndriRetMethod::setParams(Parameters *parms) {
  params = parms;
  // update the smoothing rules.
  std::vector<std::string> smoothingRules;
  // RetParamManager only puts in one...
  string rule = params->get("rule");
  smoothingRules.push_back(rule);
  env->setScoringRules( smoothingRules );
}

void lemur::retrieval::IndriRetMethod::setStopwords(const string &stopfile) {
  if (stopfile.empty()) return;
  std::vector<std::string> stopwords;
  ifstream ifs;
  ifs.open(stopfile.c_str());
  if (ifs.fail()) {
    cerr << "lemur::retrieval::IndriRetMethod::setStopwords open file " << stopfile
         << " failed." << endl; 
    return;
  }
  params->set("stopper","");
  Parameters myParams = params->get("stopper");
  string word;
  while (ifs >> word) {
    // put in param object too so RM expander will see them if need be.
    stopwords.push_back(word);
    myParams.append("word").set(word);
  }
  env->setStopwords(stopwords);
}

  
void lemur::retrieval::IndriRetMethod::scoreCollection(const QueryRep &qry, 
                                                       IndexedRealVector &results) {
  const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qry);
  // should prune coming in.
  scoreCollection(tq->getQuery(), results );
}

void lemur::retrieval::IndriRetMethod::scoreCollection(const string &qry, 
                                                       IndexedRealVector &results) {
  // should prune coming in.
  //  int resultsRequested = ind.docCount();
  int resultsRequested = params->get("count", 1000);
  std::vector<ScoredExtentResult> qRes;
  qRes = env->runQuery( qry, resultsRequested );
  // collect results into list (may not be full size of collections.
  results.clear();
  for( unsigned int i = 0; i < qRes.size(); i++ ) {
    // because we know there is only one repository, we
    // can use the indri cooked document id
    results.PushValue(qRes[i].document, qRes[i].score);
  }
}


void lemur::retrieval::IndriRetMethod::scoreDocSet(const QueryRep &qry, const DocIDSet &docSet, IndexedRealVector &results)
{
  vector<DOCID_T> ids;
  docSet.startIteration();
  while (docSet.hasMore()) {
    int docID;
    double prevScore;
    docSet.nextIDInfo(docID, prevScore);
    ids.push_back(docID);    
  }
  int resultsRequested = params->get("count", 1000);
  const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qry);
  std::vector<ScoredExtentResult> qRes;
  qRes = env->runQuery( tq->getQuery(), ids, resultsRequested );

  results.clear();
  for( unsigned int i = 0; i < qRes.size(); i++ ) {
    // because we know there is only one repository, we
    // can use the indri cooked document id
    results.PushValue(qRes[i].document, qRes[i].score);
  }
}


double lemur::retrieval::IndriRetMethod::scoreDoc(const QueryRep &qry, DOCID_T docID) {
  vector<DOCID_T> ids;
  ids.push_back(docID);
  int resultsRequested = 1;
  const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qry);
  std::vector<ScoredExtentResult> qRes;
  qRes = env->runQuery( tq->getQuery(), ids, resultsRequested );
  return(qRes[0].score);
}

// RM expand
void lemur::retrieval::IndriRetMethod::updateQuery(QueryRep &qryRep, const DocIDSet &relDocs) {

  std::vector<ScoredExtentResult> results;
  //Convert DocIDSet (document & score).
  ScoredExtentResult res;
  relDocs.startIteration();
  while (relDocs.hasMore()) {
    int docID;
    double relPr;
    relDocs.nextIDInfo(docID, relPr);
    res.document = docID;
    res.score = relPr;
    results.push_back(res);
  }
  // have to handle fbDocs == -1 for RelFBEval case
  int fbDocs = params->get( "fbDocs" , 10 );
  if (fbDocs == -1) {
    fbDocs = results.size();
  }
  if (((int)results.size()) > fbDocs) {
      results.resize(fbDocs);
  }
  // make sure RMExpander has the right value for number of documents.
  params->set("fbDocs", (int)results.size());
  const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qryRep);
  const string originalQuery = tq->getQuery();
  indri::query::RMExpander rm(env, *params);
  std::string expandedQuery = rm.expand( originalQuery , results );
  tq->updateQuery(expandedQuery);
}
