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

// David Fisher, Peter Amstutz
// init: 05/22/2002
// Flat file backend for storing clustering information
#include "FlatFileClusterDB.hpp"


// The ClusterDB.
lemur::cluster::FlatFileClusterDB::FlatFileClusterDB(const lemur::api::Index *ind, 
                                                     const string &name,
                                                     double threshold,
                                                     enum ClusterParam::simTypes simType,
                                                     enum ClusterParam::clusterTypes clusterType,
                                                     enum ClusterParam::docModes docMode)
  : ClusterDB(ind, threshold, simType, clusterType, docMode) {
  init(name);
}

void lemur::cluster::FlatFileClusterDB::init(const string &name) {
  // the clusters themselves.
  clusterDB = name + ".cl";
  doc2cluster = new int [numDocs + 1];
  for (int i = 0; i <= numDocs; i++) {
    doc2cluster[i] = 0;
  }
  clusterCount = 1;
  clusterIdCounter = 1;
  readClusterDB();
}

lemur::cluster::FlatFileClusterDB::~FlatFileClusterDB() {
  writeClusterDB();
  delete[](doc2cluster);
}

// May return NULL.
lemur::cluster::Cluster* lemur::cluster::FlatFileClusterDB::getCluster(int clusterId) const {
  if(clusters.size() <= clusterId) // id too big.
    return NULL;
  else
    return clusters[clusterId];
}

vector<lemur::cluster::Cluster*> lemur::cluster::FlatFileClusterDB::getDocCluster(lemur::api::DOCID_T docId) const {
  vector<Cluster*> v;
  if(doc2cluster[docId] > 0 && clusters[doc2cluster[docId]])
    v.push_back(clusters[doc2cluster[docId]]);
  return v;
}


lemur::cluster::Cluster* lemur::cluster::FlatFileClusterDB::newCluster() {
  int clusterID = clusterIdCounter++;
  clusterCount++;
  Cluster *newCluster = allocateCluster(clusterID);
  if (clusters.size() <= clusterID) clusters.resize(clusterID+1);
  clusters[clusterID] = newCluster;
  return newCluster;
}

vector<int> lemur::cluster::FlatFileClusterDB::getDocClusterId(lemur::api::DOCID_T docId) const {
  vector<int> v;
  if(doc2cluster[docId] > 0) 
    v.push_back(doc2cluster[docId]);
  return v;
}

int lemur::cluster::FlatFileClusterDB::addToCluster(lemur::api::DOCID_T docId, int clusterId, double score) {
  Cluster *cluster = clusters[clusterId];
  addToCluster(docId, cluster, score);
  return clusterId;
}

int lemur::cluster::FlatFileClusterDB::addToCluster(lemur::api::DOCID_T docId, lemur::cluster::Cluster *cluster, double score)
{
  doc2cluster[docId] = cluster->getId();
  ClusterElt fred;
  fred.id = docId;
  fred.myType = DOC_ELT;
  cluster->add(fred);
  return cluster->getId();
}

// remove doc from cluster
int lemur::cluster::FlatFileClusterDB::removeFromCluster(lemur::api::DOCID_T docId, int clusterId) {
  Cluster *cluster = clusters[clusterId];
  ClusterElt fred;
  fred.id = docId;
  fred.myType = DOC_ELT;
  cluster->remove(fred);
  doc2cluster[docId] = 0;
  return 0;
}

// split cluster
vector<int> lemur::cluster::FlatFileClusterDB::splitCluster(int clusterId, int numParts) {
  vector<int> retval;
  Cluster *cluster = clusters[clusterId];
  vector<Cluster *> newClusters = cluster->split(numParts);
  // delete the old cluster
  delete(cluster);
  clusterCount--;
  clusters[clusterId] = NULL;
  // Renumber the new clusters.
  char *myName = new char[NAMESIZE];
  int numNew = newClusters.size();
  // make room
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
    for (int j = 0; j < docs.size(); j++) {
      doc2cluster[docs[j]] = newId;
    }
  }
  clusterCount += numNew;
  delete[](myName);
  return retval;
}

// delete a cluster
int lemur::cluster::FlatFileClusterDB::deleteCluster (lemur::cluster::Cluster *target) {
  return deleteCluster(target->getId());
}
int lemur::cluster::FlatFileClusterDB::deleteCluster(int clusterID) {
  Cluster *cl2 = clusters[clusterID];
  vector <lemur::api::DOCID_T> docs = cl2->getDocIds();
  for (int i = 0; i < docs.size(); i++) {
    doc2cluster[docs[i]] = 0;
  }
  delete(cl2);
  clusters[clusterID] = NULL;
  clusterCount--;
  return clusterID;
}

// merge clusters
int lemur::cluster::FlatFileClusterDB::mergeClusters(int c1, int c2) {
  Cluster *cl1 = clusters[c1];
  Cluster *cl2 = clusters[c2];
  cl1->merge(cl2);  
  vector <lemur::api::DOCID_T> docs = cl2->getDocIds();
  for (int i = 0; i < docs.size(); i++) {
    doc2cluster[docs[i]] = c1;
  }
  delete(cl2);
  clusters[c2] = NULL;
  clusterCount--;
  return c1;
}

void lemur::cluster::FlatFileClusterDB::readClusterDB()
{
  ifstream in;
  int index;
  int maxid = -1;
  Cluster *cluster;
  in.open(clusterDB.c_str(), ios::binary | ios::in);
  if (in.fail()) {
    in.close();
    return;
  }
  while (! in.eof()) {
    cluster = allocateCluster(0);
    if (!cluster->read(in)) {
      delete(cluster);
      break;
    }
    index = cluster->getId();
    if (index > maxid) maxid = index;
    if (clusters.size() <= index) 
      clusters.resize(index + 1);
    clusters[index] = cluster;
    clusterCount++;
    // update doc2cluster table.
    vector <lemur::api::DOCID_T> docs = cluster->getDocIds();
    for (int i = 0; i < docs.size(); i++) {
      doc2cluster[docs[i]] = index;
    }
  }
  in.close();
  clusterIdCounter = maxid + 1;
}

void lemur::cluster::FlatFileClusterDB::writeClusterDB(){
  ofstream out;
  int i;
  Cluster *cl;
  out.open(clusterDB.c_str(), ios::out | ios::binary);
  int max = maxID();
  for (i = 1; i <= max; i++) {
    cl = getCluster(i);
    if (cl) {
      cl->write(out);
    }
  }
  out.close();
}
