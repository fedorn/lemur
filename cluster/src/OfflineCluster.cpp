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
// init: 02/03/2003
// Offline clustering algorithms.
#include "OfflineCluster.hpp"

lemur::cluster::OfflineCluster::OfflineCluster(const lemur::api::Index &ind,
                                               enum ClusterParam::simTypes simType,
                                               enum ClusterParam::clusterTypes clusterType,
                                               enum ClusterParam::docModes docMode)
  : index(ind) {
  // Setup similarity method and cluster factories.
  sim = SimFactory::makeSim(index, simType);
  factory = new ClusterFactory(index, *sim, clusterType, docMode);
}

lemur::cluster::OfflineCluster::~OfflineCluster() {
  delete(sim);
  delete(factory);
}

lemur::cluster::Cluster *lemur::cluster::OfflineCluster::chooseSplit(vector<Cluster *> *working) {
  int tmp, max = 0;
  Cluster *retval = NULL;
  vector<Cluster *>::iterator it, best;
  for (it = working->begin(); it != working->end(); it++) {
    tmp = (*it)->getSize();
    if (tmp > max) {
      max = tmp;
      retval = *it;
      best = it;
    }
  }
  working->erase(best);
  return retval;  
}

double lemur::cluster::OfflineCluster::scoreSet(vector<Cluster *> *working) {
  double retval = 0;
  if (working != NULL) {
    for (int i = 0; i < working->size(); i++) {
      retval += (*working)[i]->sum2();
    }
  }
  return retval;
}

vector<lemur::cluster::Cluster*> *lemur::cluster::OfflineCluster::bisecting_kMeans(vector<lemur::api::DOCID_T> docIds, 
                                                                                   int numParts,
                                                                                   int numIters,
                                                                                   int maxIters) {
  vector<Cluster *> *finalClusters = new vector<Cluster*>;
  vector<Cluster *> *bestSplit = NULL, *split, 
    *working = new vector<Cluster*>;
  int clusterCount = 1;
  Cluster *cluster = factory->allocateCluster(clusterCount);
  //All docs into 1 cluster
  cluster->add(docIds);
  working->push_back(cluster);
  //repeat
  do {
    //  Pick cluster to split (eg largest).
    // removes it from the vector.
    cluster = chooseSplit(working);
    //  repeat
    for (int i = 0; i < numIters; i++) {
      //    Use k-means with k=2
      //  iter times, keeping split with highest overall similarity
      //   (squared length of cluster centroid, sum2).
      split = kMeans(cluster, 2, maxIters);
      double s1 = scoreSet(bestSplit);
      double s2 = scoreSet(split);
      if (s2 > s1) {
        if (bestSplit != NULL) {
          // clean up
          for (int j = 0; j < bestSplit->size(); j++)
            delete((*bestSplit)[j]);
          delete(bestSplit);
        }
        bestSplit = split;
        split = NULL;
      } else {
        // clean up
        for (int j = 0; j < split->size(); j++)
          delete((*split)[j]);
        delete(split);
        split = NULL;
      }
    }
    // put best back into working.
    for (int j = 0; j < bestSplit->size(); j++)      
      working->push_back((*bestSplit)[j]);
    bestSplit = NULL; 
    //until enough clusters.
    clusterCount = working->size();
    delete (cluster);
  } while (clusterCount < numParts);

  // clean up ids and insert into retval
  vector<Cluster *>::iterator it;
  int counter = 1;
  char *myName = new char[NAMESIZE];
  for (it = working->begin(); it != working->end(); it++) {
    Cluster * item = *it;
    sprintf(myName, "%d", counter);
    item->setId(counter++);
    item->setName(myName);
    finalClusters->push_back(item);
  }
  delete[](myName);
  delete(working);
  return finalClusters;
}

vector<lemur::cluster::Cluster*> *lemur::cluster::OfflineCluster::kMeans(Cluster *cluster, int numParts,
                                                                         int maxIters) {
  return kMeans(cluster->getDocIds(), numParts, maxIters);
}
 
vector<lemur::cluster::Cluster*> *lemur::cluster::OfflineCluster::kMeans(vector<lemur::api::DOCID_T> docIds, int numParts,
                                                                         int maxIters) {
  vector<Cluster*> *clusters = new vector<Cluster*>;
  // cluster elements
  bool done = false;
  int numDocs = docIds.size();
  int numIters = 0;
  // make sure there are enough docs.
  if (numDocs < numParts){
    // pathological case
    numParts = numDocs;
  }
  
  ClusterRep **docReps = new ClusterRep *[numDocs];
  Cluster **firstPass = new Cluster *[numParts];
  Cluster **secondPass = new Cluster *[numParts];
  // build reps
  int i;
  for (i = 0; i < numDocs; i++) {
    int docId = docIds[i];
    lemur::api::TermInfoList *tList = index.termInfoList(docId);
    docReps[i] = new ClusterRep(tList, index);
    sim->weigh(docReps[i]);
    delete(tList);
  }
  // Choose seeds
  vector <lemur::api::DOCID_T> seeds = selectSeeds(docIds, numParts);  
  for (i = 0; i < numParts; i++){
    // init first pass and second pass arrays.
    firstPass[i] = factory->allocateCluster(i + 1);
    ClusterElt fred;
    fred.id = seeds[i];
    fred.myType = DOC_ELT;
    firstPass[i]->add(fred);
    secondPass[i] = factory->allocateCluster(i + 1);
  }
  // iterate until unchanged or have had enough...
  while (! done && numIters < maxIters) {
    numIters++;
    // assign all elts to closest
    int i;
    for (i = 0; i < numDocs; i++) {
      double score = 0, maxScore = 0;
      int idx = 0;
      for (int j = 0; j < numParts; j++) {
        score = firstPass[j]->score(docReps[i]);
        if (score > maxScore) {
          maxScore = score;
          idx = j;
        }
      }
      ClusterElt fred;
      fred.id = docIds[i];
      fred.myType = DOC_ELT;
      secondPass[idx]->add(fred);
    }
    done = compareClusterSets(firstPass, secondPass, numParts);
    // relocate cluster reps
    for (i = 0; i < numParts; i++) {
      delete(firstPass[i]);
      firstPass[i] = secondPass[i];
      secondPass[i] = factory->allocateCluster(i + 1);
    }
  }
  // load final clusters into clusters
  // Need a fresh one each time, caller deletes.
  for (i = 0; i < numParts; i++) {
    clusters->push_back(firstPass[i]);
    firstPass[i] = NULL;
    delete(secondPass[i]);
    secondPass[i] = NULL;
  }
  // clean up
  for (i = 0; i < numDocs; i++) {
    delete(docReps[i]);
  }
  delete[](docReps);
  // first pass was copied into clusters and nulled.    
  delete[](firstPass);
  // second pass was deleted and nulled.
  delete[](secondPass);
  // load into clusters and return
  return clusters;
}

bool lemur::cluster::OfflineCluster::compareClusterSets(Cluster **first, Cluster **second, 
                                                        int n) {
  bool match = true;
  for (int i = 0; i < n && match; i++) {
    vector<ClusterElt> fIds = *(first[i]->getIds());
    vector<ClusterElt> sIds = *(second[i]->getIds());
    if (fIds != sIds) {
      match = false;
    }
  }
  return match;
}

vector <lemur::api::DOCID_T> lemur::cluster::OfflineCluster::selectSeeds(vector<lemur::api::DOCID_T> docIds, int num) {
  // Chose num random elements without duplicates from docIds
  int max = docIds.size() - 1;
  if (max < num) return docIds; // all of them have to go back.
  vector <lemur::api::DOCID_T> results;
  vector <lemur::api::DOCID_T> tmp = docIds;
  while (results.size() < num) {
    int idx = rand()%max;
    results.push_back(tmp[idx]);
    tmp.erase(tmp.begin() + idx);
    max--;
  }
  return results;
}

