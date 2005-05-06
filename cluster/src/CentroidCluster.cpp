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
// init: 11/18/2002
#include "CentroidCluster.hpp"

lemur::cluster::CentroidCluster::CentroidCluster(int cid, 
                                                 const lemur::api::Index &ind, 
                                                 const SimilarityMethod &sim) :
  Cluster(cid, ind, sim) {
  centroid = NULL;
  weightedCentroid = NULL;
}

double lemur::cluster::CentroidCluster::score(const ClusterRep *rep) const {
  // should not happen.
  if (size == 0) return 0; 
  double s = similarity.similarity(rep, weightedCentroid);
  return s;
}

lemur::cluster::ClusterRep *lemur::cluster::CentroidCluster::getClusterRep() 
  const {
  return new ClusterRep(weightedCentroid);
}

void lemur::cluster::CentroidCluster::updateCentroid(lemur::utility::FloatFreqVector *v, bool first) {
  if (first) {
    centroid = new lemur::utility::FloatFreqVector(v);
  } else {
    delete(weightedCentroid);
    centroid->addVec(v);
  }
  weightedCentroid = new ClusterRep(centroid, ind);
  weightedCentroid->weigh(1.0/size);
  similarity.weigh(weightedCentroid);
}

void lemur::cluster::CentroidCluster::add(const ClusterElt &elt) {  
  bool firstOne = (size == 0);
  Cluster::add(elt); 
  ClusterRep *rep = new ClusterRep(elt.id, ind);
  // VIOLATING ENCAPSULATION. Bleah!
  updateCentroid(rep->getRep(), firstOne);
  delete(rep);
}

void lemur::cluster::CentroidCluster::add(vector<lemur::api::DOCID_T> docids) {
  bool firstOne = (size == 0);
  // don't use add for efficiency's sake (single update of centroid).
  ClusterElt fred;
  for (int i = 0; i < docids.size(); i++) {
    fred.id = docids[i];
    fred.myType = DOC_ELT;
    ids.push_back(fred);
    size++;
  }
  ClusterRep *rep = new ClusterRep(docids, ind);
  // VIOLATING ENCAPSULATION. Bleah!
  updateCentroid(rep->getRep(), firstOne);
  delete(rep);
}


void lemur::cluster::CentroidCluster::remove(const ClusterElt &elt) {
  vector<ClusterElt>::iterator it = ids.begin();
  while (it != ids.end() && 
         ((*it).myType != elt.myType || (*it).id != elt.id)) it++;
  if (it != ids.end()) {
    ClusterRep *rep = new ClusterRep((*it).id, ind);
    centroid->subtract(rep->getRep());
    // reweigh.
    delete(weightedCentroid);
    size--;
    if (size == 0){
      weightedCentroid = NULL;
    } else {
      weightedCentroid = new ClusterRep(centroid, ind);
      weightedCentroid->weigh(1.0/size);
      similarity.weigh(weightedCentroid);
    }
    delete(rep);
    ids.erase(it);
  }
}

lemur::utility::FloatFreqVector *lemur::cluster::CentroidCluster::readCentroid() {
  vector <lemur::api::DOCID_T> dids = getDocIds();
  return new lemur::utility::FloatFreqVector(ind, dids);
}

bool lemur::cluster::CentroidCluster::read(ifstream &in) {
  // Augment the basic superclass read
  bool retval = Cluster::read(in);
  if (retval) {
    // Put centroid in memory...
    centroid = readCentroid();
    weightedCentroid = new ClusterRep(centroid, ind);
    weightedCentroid->weigh(1.0/size);
    similarity.weigh(weightedCentroid);
  }
  return retval;
}
