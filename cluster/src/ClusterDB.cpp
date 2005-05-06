/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

// David Fisher
// init: 05/22/2002
// Abstract interface for clustering algorithms.
#include "ClusterDB.hpp"
// The ClusterDB.


lemur::api::ClusterDB::ClusterDB(const lemur::api::Index *ind,      
                                 double threshold,
                                 enum ClusterParam::simTypes simType,
                                 enum ClusterParam::clusterTypes clusterType,
                                 enum ClusterParam::docModes docMode)
  : index(ind) {
  // Setup similarity method and cluster factories.
  sim = lemur::cluster::SimFactory::makeSim(*ind, simType);
  factory = new lemur::cluster::ClusterFactory(*index, *sim, clusterType, 
                                               docMode);
  numTerms = index->termCountUnique();
  numDocs = index->docCount();
  // This needs a factory later (or forget it....).
  thresh = new lemur::cluster::ThresholdFcn(threshold);
}

lemur::api::ClusterDB::~ClusterDB() {
  for (int i = 0; i < clusters.size(); i++)
    delete(clusters[i]);
  clusters.clear();
  delete(sim);
  delete(factory);
  delete(thresh);
}

void lemur::api::ClusterDB::printClusters() const {
  // skip the empty 0th cluster
  int max = maxID();
  for (int i = 1; i <= max; i++) {
    lemur::cluster::Cluster *cl = getCluster(i);
    if (cl) cl->print();
  }
}

int lemur::api::ClusterDB::cluster(lemur::api::DOCID_T docId) {
  double score;
  return cluster(docId, score);
}

int lemur::api::ClusterDB::cluster(lemur::api::DOCID_T docId, 
                                   double &finalScore) {
  lemur::cluster::Cluster *cluster;
  int cid, myCluster;
  double score;
  TermInfoList *tList = index->termInfoList(docId);
  lemur::cluster::ClusterRep *docRep = new lemur::cluster::ClusterRep(tList,
                                                                      *index);
  sim->weigh(docRep);
  int numClusters = countClusters();
  int maxId = maxID();
  IndexedRealVector results(numClusters);
  results.clear();

  for (int i = 1; i <= maxId; i++) {
    cluster = getCluster(i);
    if (cluster)
      results.PushValue(i, cluster->score(docRep));
  }
  if (results.size() == 0) {
    cluster = newCluster();
    score = 1.0;
  } else {
    results.Sort();
    myCluster = results[0].ind;
    score = results[0].val;
    cluster = getCluster(myCluster);
    if (! thresh->threshold(score, cluster->getSize())) {
      // new cluster
      cluster = newCluster();
      score = 1.0;
    }
  }
  addToCluster(docId, cluster, score);
  finalScore = score;
  cid = cluster->getId();
  delete (docRep);
  delete(tList);
  return cid;
}

/* copy the cluster, allocating it in the db */
int lemur::api::ClusterDB::addCluster(lemur::cluster::Cluster *oldCluster) {
  lemur::cluster::Cluster *cluster = newCluster(); 
  double score = 1.0;
  vector <lemur::api::DOCID_T> docs = oldCluster->getDocIds();
  for (vector<lemur::api::DOCID_T>::iterator d = docs.begin(); 
       d != docs.end(); d++) {
    addToCluster(*d, cluster, score);
  }
  return cluster->getId();
}

lemur::cluster::Cluster* lemur::api::ClusterDB::allocateCluster(int clusterID)
  const {
  return factory->allocateCluster(clusterID);
}

string lemur::api::ClusterDB::getKeyWords(int cid, int numTerms) const {
  lemur::cluster::Cluster * cl = getCluster(cid);
  if (cl) return cl->getKeyWords(numTerms);
  return NULL;
}
