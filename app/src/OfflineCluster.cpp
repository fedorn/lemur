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
#include "IndexManager.hpp"
#include "OfflineCluster.hpp"
#include "ClusterParam.hpp"
// Offline Clustering

/*! \page OffLineCluster Offline Clustering
  This example application (<tt>OfflineCluster.cpp</tt>) demonstrates the 
  basic offline clustering task. Provides k-means and bisecting k-means 
  partitional clustering. It will run each algorithm on the first 100 
  documents in the index (or all of them if less than 100) and print out 
  the results. 

  The parameters accepted by OfflineCluster are:
  <ul>
  <li> index -- the index to use. Default is none.
  <li>  clusterType -- Type of cluster to use, either agglomerative or 
  centroid.  Centroid is agglomerative using mean which trades memory use 
  for speed of clustering. Default is centroid.
  <li>  simType -- The similarity metric to use. Default is cosine 
  similarity (COS), which is the only implemented method.
  <li>  docMode -- The integer encoding of the scoring method to use for the
  agglomerative cluster type. The default is max (maximum).
  The choices are:
<ul>
<li> max -- Maximum score over documents in a cluster.
<li> mean -- Mean score over documents in a cluster. This is identical to the 
centroid cluster type.
<li> avg -- Average score over documents in a cluster.
<li> min -- Minimum score over documents in a cluster.
</ul>
<li> numParts -- Number of partitions to split into. Default is 2
<li>    maxIters -- Maximum number of iterations for k-means. Default is 100.
<li>  bkIters -- Number of k-means iterations for bisecting k-means. 
Default is 5.
  </ul>
*/
/*
  author: dmf
 */

// get application parameters
void GetAppParam() {
  ClusterParam::get();
}

int AppMain(int argc, char * argv[]) {
  if (argc > 2) {
    cerr << "Usage: OfflineCluster <parameter file>" << endl;
    return -1;
  }
  int i;
  Index *myIndex;
  try {
    myIndex  = IndexManager::openIndex(ClusterParam::databaseIndex);
  } catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("OfflineCluster", 
		    "Can't open index, check parameter index");
  }
  // construct cluster method.
  OfflineCluster* clusterDB = new OfflineCluster(*myIndex,
						 ClusterParam::simType,
						 ClusterParam::clusterType,
						 ClusterParam::docMode);
  // crank through the collection
  int numDocs = myIndex->docCount();
  if (numDocs > 100) numDocs = 100;
  vector <int> toCluster;
  for (i = 1; i <= numDocs; i++) {
    toCluster.push_back(i);
  }

  cout << "Using kmeans on " << numDocs << " documents..." << endl;
  vector <Cluster *> *clusters = clusterDB->kMeans(toCluster, 
						   ClusterParam::numParts,
						   ClusterParam::maxIters);

  for (i = 0; i < clusters->size(); i++) {
    (*clusters)[i]->print();
    delete((*clusters)[i]);
  }
  delete(clusters);
  cout << "Using bisecting kmeans on " << numDocs << " documents..." << endl;
  clusters = clusterDB->bisecting_kMeans(toCluster, 
					 ClusterParam::numParts,
					 ClusterParam::numIters,
					 ClusterParam::maxIters);
  for (i = 0; i < clusters->size(); i++) {
    (*clusters)[i]->print();
    delete((*clusters)[i]);
  }
  delete(clusters);
  delete clusterDB;
  delete myIndex;
  return 0;
}
