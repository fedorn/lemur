/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#include "IndriRetMethod.hpp"
#include "indri/QueryEnvironment.hpp"
#include "indri/RMExpander.hpp"

IndriRetMethod::IndriRetMethod(const Index &dbIndex) : 
  RetrievalMethod(dbIndex) {
  // if it isn't an indri repository, die screaming now.
  const LemurIndriIndex *rep = dynamic_cast<const LemurIndriIndex *>(&ind);
  if (rep == NULL) {
    LEMUR_THROW(LEMUR_RUNTIME_ERROR, 
		"IndriRetMethod requires a LemurIndriIndex.");
  }

  env = new QueryEnvironment();
  env->addIndex(rep->getRepositoryName());
}

/// clean up.
IndriRetMethod::~IndriRetMethod() {
  delete(env);
}

void IndriRetMethod::setParams(Parameters *parms) {
    params = parms;
    // update the smoothing rules.
    std::vector<std::string> smoothingRules;
    // RetParamManager only puts in one...
    string rule = params->get("rule");
    smoothingRules.push_back(rule);
    env->setScoringRules( smoothingRules );
  }

void IndriRetMethod::setStopwords(const string &stopfile) {
  if (stopfile.empty()) return;
  std::vector<std::string> stopwords;
  ifstream ifs;
  ifs.open(stopfile.c_str());
  if (ifs.fail()) {
    cerr << "IndriRetMethod::setStopwords open file " << stopfile
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

  
void IndriRetMethod::scoreCollection(const QueryRep &qry, 
			       IndexedRealVector &results) {
  const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qry);
  // should prune coming in.
  scoreCollection(tq->getQuery(), results );
}

void IndriRetMethod::scoreCollection(const string &qry, 
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


void IndriRetMethod::scoreDocSet(const QueryRep &qry, const DocIDSet &docSet, IndexedRealVector &results)
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


double IndriRetMethod::scoreDoc(const QueryRep &qry, DOCID_T docID) {
  vector<DOCID_T> ids;
  ids.push_back(docID);
  int resultsRequested = 1;
  const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qry);
  std::vector<ScoredExtentResult> qRes;
  qRes = env->runQuery( tq->getQuery(), ids, resultsRequested );
  return(qRes[0].score);
}

// RM expand
void IndriRetMethod::updateQuery(QueryRep &qryRep, const DocIDSet &relDocs) {
  /*
    RMExpander rm;
    Convert DocIDSet to std::vector<ScoredExtentResult> (document & score).
    std::string expandedQuery = rm.expand( originalQuery , results );

    update Qry to hold expanded string
    const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qryRep);
    tq->updateQuery(expandedQuery);
   */

  RMExpander rm(env, *params);

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
  const IndriQueryModel *tq = dynamic_cast<const IndriQueryModel *>(&qryRep);
  const string originalQuery = tq->getQuery();
  std::string expandedQuery = rm.expand( originalQuery , results );
  tq->updateQuery(expandedQuery);
}
