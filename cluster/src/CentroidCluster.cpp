/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

// David Fisher
// init: 11/18/2002
#include "CentroidCluster.hpp"

CentroidCluster::CentroidCluster(int cid, const Index &ind, 
				 const SimilarityMethod &sim) :
  Cluster(cid, ind, sim) {
  centroid = NULL;
  weightedCentroid = NULL;
}

double CentroidCluster::score(const ClusterRep *rep) const {
  // should not happen.
  if (size == 0) return 0; 
  double s = similarity.similarity(rep, weightedCentroid);
  return s;
}

ClusterRep *CentroidCluster::getClusterRep() const {
  return new ClusterRep(weightedCentroid);
}

void CentroidCluster::updateCentroid(FloatFreqVector *v, bool first) {
  if (first) {
    centroid = new FloatFreqVector(v);
  } else {
    delete(weightedCentroid);
    centroid->addVec(v);
  }
  weightedCentroid = new ClusterRep(centroid, ind);
  weightedCentroid->weigh(1.0/size);
  similarity.weigh(weightedCentroid);
}

void CentroidCluster::add(const ClusterElt &elt) {  
  bool firstOne = (size == 0);
  Cluster::add(elt); 
  ClusterRep *rep = new ClusterRep(elt.id, ind);
  // VIOLATING ENCAPSULATION. Bleah!
  updateCentroid(rep->getRep(), firstOne);
  delete(rep);
}

void CentroidCluster::add(vector<int> docids) {  
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


void CentroidCluster::remove(const ClusterElt &elt) {
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

FloatFreqVector *CentroidCluster::readCentroid() {
  vector <int> dids = getDocIds();
  return new FloatFreqVector(ind, dids);
}

bool CentroidCluster::read(ifstream &in) {
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
