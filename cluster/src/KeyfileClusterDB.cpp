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
// init: 05/11/2004
// Keyfile backend for storing clustering information
#include "KeyfileClusterDB.hpp"

// max docs in a cluster.
#define MAX_IDS 1024 * 4

// The ClusterDB.
lemur::cluster::KeyfileClusterDB::KeyfileClusterDB(const lemur::api::Index *ind, 
                                                   const string &name,
                                                   double threshold,
                                                   enum ClusterParam::simTypes simType,
                                                   enum ClusterParam::clusterTypes cType,
                                                   enum ClusterParam::docModes docMode)
  : ClusterDB(ind, threshold, simType, cType, docMode) {
  init(name);
}

void lemur::cluster::KeyfileClusterDB::init(const string &name) {
  // the clusters themselves.
  string clusterDB = name + ".cl-key";
  string d2cDB = name + ".d2c-key";
  int max;
  // open or create
  try {
    clustersKey.open(clusterDB);
    doc2cluster.open(d2cDB);
    // if either throws, need to create.
  } catch (lemur::api::Exception e) {
    clustersKey.create(clusterDB);
    doc2cluster.create(d2cDB);
    max = 0;
    clustersKey.put("MAXID", &max, sizeof(max));
    clustersKey.put("NUMCLUSTERS", &max, sizeof(max));
  }
  // max id and num clusters set.
  max = maxID();
  clusters.resize(max + 1);
  for (int i = 0; i < clusters.size(); i++) {
    clusters[i] = NULL;
  }
}

lemur::cluster::KeyfileClusterDB::~KeyfileClusterDB() {
  doc2cluster.close();
  clustersKey.close();
}

// May return NULL.
lemur::cluster::Cluster* lemur::cluster::KeyfileClusterDB::getCluster(int clusterId) const {
  
  // have to grow?
  if (clusters.size() <= clusterId) {
    int oldsize = clusters.size();
    clusters.resize(clusterId + 1);
    for (; oldsize < clusterId + 1; oldsize++) {
      clusters[oldsize] = NULL;
    }
  }

  Cluster *retVal = clusters[clusterId];
  // not already loaded.
  if (retVal == NULL) {
    lemur::api::DOCID_T buffer[MAX_IDS]; // size
    int actual;
    bool success = clustersKey.get(clusterId, buffer, actual, sizeof(buffer));
    if(success) {
      // create a cluster from a buffer. alternative read method here.
      clusters[clusterId] = allocateCluster(clusterId);
      retVal = clusters[clusterId];
      // easier just to add each doc into a fresh empty cluster.
      vector <lemur::api::DOCID_T> dids;
      for (int i = 0; i < actual/sizeof(lemur::api::DOCID_T) ; i++) {
        dids.push_back(buffer[i]);
      }
      retVal->add(dids);
    }
  }
  return retVal;
}

vector<lemur::cluster::Cluster*> lemur::cluster::KeyfileClusterDB::getDocCluster(lemur::api::DOCID_T docId) const {
  vector<Cluster*> v;
  vector<int> d2c = getDocClusterId(docId);
  for (int i = 0; i < d2c.size(); i++)
    v.push_back(getCluster(d2c[i]));
  return v;
}


lemur::cluster::Cluster* lemur::cluster::KeyfileClusterDB::newCluster() {
  // update table
  int clusterIdCounter = maxID() ;
  int clusterCount = countClusters();
  clusterIdCounter++;
  clusterCount++;
  if (clusters.size() <= clusterIdCounter) {
    clusters.resize(clusterIdCounter + 1);
  }
  Cluster *newCluster = allocateCluster(clusterIdCounter);
  clustersKey.put("MAXID", &clusterIdCounter, sizeof(int));
  clustersKey.put("NUMCLUSTERS", &clusterCount, sizeof(int));
  clusters[clusterIdCounter] = newCluster;
  return newCluster;
}

vector<int> lemur::cluster::KeyfileClusterDB::getDocClusterId(lemur::api::DOCID_T docId) const {
  vector<int> v;
  int buffer[MAX_IDS];
  int actual;
  bool success = doc2cluster.get(docId, buffer, actual, sizeof(buffer));
  // won't be more than 1 at this point.
  if (success)
    for (int *p = buffer; p < (buffer + actual/sizeof(int)); p++)
      v.push_back(*p);
  return v;
}

int lemur::cluster::KeyfileClusterDB::addToCluster(lemur::api::DOCID_T docId, int clusterId, double score) {
  Cluster *cluster = getCluster(clusterId);
  return addToCluster(docId, cluster, score);
}

int lemur::cluster::KeyfileClusterDB::addToCluster(lemur::api::DOCID_T docId, Cluster *cluster, double score)
{
  int cid = cluster->getId();

  doc2cluster.put(docId, &cid, sizeof(int));
  ClusterElt fred;
  fred.id = docId;
  fred.myType = DOC_ELT;
  cluster->add(fred);

  lemur::api::DOCID_T buffer[MAX_IDS]; // size element here?
  int actual;
  bool success = clustersKey.get(cid, buffer, actual, sizeof(buffer));
  if (!success)
    actual = 0;
  buffer[actual/sizeof(lemur::api::DOCID_T)] = docId;
  actual += sizeof(lemur::api::DOCID_T);
  clustersKey.put(cid, buffer, actual);
  return cid;
}

// remove doc from cluster
int lemur::cluster::KeyfileClusterDB::removeFromCluster(lemur::api::DOCID_T docId, int clusterId) {
  // doc2cluster.put, clustersKey.get, clustersKey.put
  Cluster *cluster = getCluster(clusterId);
  ClusterElt fred;
  fred.id = docId;
  fred.myType = DOC_ELT;
  cluster->remove(fred);
  doc2cluster.remove(docId);

  lemur::api::DOCID_T buffer[MAX_IDS]; // size element here?
  int actual;
  // update clustersKey  
  vector<lemur::api::DOCID_T> docids = cluster->getDocIds();
  int j = 0;
  buffer[0] = docids.size();
  for (vector<lemur::api::DOCID_T>::iterator it = docids.begin(); it != docids.end(); it++)
    buffer[j++] = *it;
  actual = j * sizeof(lemur::api::DOCID_T);
  clustersKey.put(clusterId, buffer, actual);
  return 0;
}

// split cluster
vector<int> lemur::cluster::KeyfileClusterDB::splitCluster(int clusterId, int numParts) {
  vector<int> retval;
  Cluster *cluster = getCluster(clusterId);
  vector<Cluster *> newClusters = cluster->split(numParts);
  // delete the old cluster
  deleteCluster(cluster);
  // Renumber the new clusters
  char *myName = new char[NAMESIZE];
  int numNew = newClusters.size();
  // make room
  int clusterIdCounter = countClusters();
  if (clusters.size() <= (clusterIdCounter + numNew)) 
    clusters.resize(clusterIdCounter + numNew + 1);
  for (int i = 0; i < numNew; i++) {
    int newId = clusterIdCounter++;
    retval.push_back(newId);
    cluster = newClusters[i];
    sprintf(myName, "%d", newId);
    cluster->setId(newId);
    cluster->setName(myName);
    clusters[newId] = cluster;
    vector <lemur::api::DOCID_T> docs = cluster->getDocIds();
    addC2D(newId, docs);
    for (vector<lemur::api::DOCID_T>::iterator it = docs.begin(); it != docs.end(); it++)
      doc2cluster.put(*it, &newId, sizeof(int));
  }
  delete[](myName);
  return retval;
}

// delete a cluster
int lemur::cluster::KeyfileClusterDB::deleteCluster (Cluster *target) {
  int clusterID = target->getId();
  vector <lemur::api::DOCID_T> docs = target->getDocIds();
  for (vector<lemur::api::DOCID_T>::iterator it = docs.begin(); it != docs.end(); it++) {
    lemur::api::DOCID_T docID = *it;
    doc2cluster.remove(docID);
  }
  clusters[clusterID] = NULL;
  int clusterCount = countClusters();
  clusterCount--;
  clustersKey.put("NUMCLUSTERS", &clusterCount, sizeof(int));
  clustersKey.remove(clusterID);
  delete(target);
  return clusterID;
}

int lemur::cluster::KeyfileClusterDB::deleteCluster(int clusterID) {
  return deleteCluster(getCluster(clusterID));
}

// merge clusters
int lemur::cluster::KeyfileClusterDB::mergeClusters(int c1, int c2) {
  //clusters.put
  Cluster *cl1 = getCluster(c1);
  Cluster *cl2 = getCluster(c2);
  vector <lemur::api::DOCID_T> docs = cl2->getDocIds();
  deleteCluster(cl2);
  cl1->add(docs);
  addC2D(c1, docs);
  for (vector<lemur::api::DOCID_T>::iterator it = docs.begin(); it != docs.end(); it++) {
    lemur::api::DOCID_T docID = *it;
    doc2cluster.put(docID, &c1, sizeof(int));
  }
  return c1;
}

void lemur::cluster::KeyfileClusterDB::addC2D(int clusterId, const vector<lemur::api::DOCID_T> &docids) {
  // update clustersKey  
  lemur::api::DOCID_T buffer[MAX_IDS]; // size element here?
  int actual;
  bool success = clustersKey.get(clusterId, buffer, actual, sizeof(buffer));
  int num = 0;
  if (success) num = actual/sizeof(lemur::api::DOCID_T);
  int j = num + 1;
  for (vector<lemur::api::DOCID_T>::const_iterator it = docids.begin(); 
       it != docids.end(); 
       it++)
    buffer[j++] = *it;
  actual = j * sizeof(lemur::api::DOCID_T);
  clustersKey.put(clusterId, buffer, actual);
}
