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


ClusterDB::ClusterDB(const Index *ind, 	    
		     double threshold,
		     enum ClusterParam::simTypes simType,
		     enum ClusterParam::clusterTypes clusterType,
		     enum ClusterParam::docModes docMode)
  : index(ind) {
  // Setup similarity method and cluster factories.
  sim = SimFactory::makeSim(*ind, simType);
  factory = new ClusterFactory(*index, *sim, clusterType, docMode);
  numTerms = index->termCountUnique();
  numDocs = index->docCount();
  // This needs a factory later (or forget it....).
  thresh = new ThresholdFcn(threshold);
}

ClusterDB::~ClusterDB() {
  for (int i = 0; i < clusters.size(); i++)
    delete(clusters[i]);
  clusters.clear();
  delete(sim);
  delete(factory);
  delete(thresh);
}

void ClusterDB::printClusters() const {
  // skip the empty 0th cluster
  int max = maxID();
  for (int i = 1; i <= max; i++) {
    Cluster *cl = getCluster(i);
    if (cl) cl->print();
  }
}

int ClusterDB::cluster(DOCID_T docId) {
  double score;
  return cluster(docId, score);
}

int ClusterDB::cluster(DOCID_T docId, double &finalScore) {
  Cluster *cluster;
  int cid, myCluster;
  double score;
  TermInfoList *tList = index->termInfoList(docId);
  ClusterRep *docRep = new ClusterRep(tList, *index);
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
int ClusterDB::addCluster(Cluster *oldCluster) {
  Cluster *cluster = newCluster(); 
  double score = 1.0;
  vector <DOCID_T> docs = oldCluster->getDocIds();
  for (vector<DOCID_T>::iterator d = docs.begin(); d != docs.end(); d++) {
    addToCluster(*d, cluster, score);
  }
  return cluster->getId();
}

Cluster* ClusterDB::allocateCluster(int clusterID) const {
  return factory->allocateCluster(clusterID);
}

string ClusterDB::getKeyWords(int cid, int numTerms) const {
  Cluster * cl = getCluster(cid);
  if (cl) return cl->getKeyWords(numTerms);
  return NULL;
}
