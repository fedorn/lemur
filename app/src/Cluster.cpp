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
#include "IndexManager.hpp"
#include "FlatFileClusterDB.hpp"
#include "KeyfileClusterDB.hpp"
#include "ClusterParam.hpp"
using namespace lemur::api;
using namespace lemur::cluster;

// Online Clustering

/*! \page Cluster Online Clustering
  This application performs the basic online 
  clustering task.
  In conjunction with an incremental indexer (such as KeyfileIncIndex), 
  it could be used for TDT 
  topic detection task. It iterates over the documents in the index, 
  assigning each document that is not in any cluster to a cluster. The 
  document id, cluster id, and score are printed to the standard output.

  The parameters accepted by Cluster are:
  <ul>
  <li> index -- the index to use. Default is none.
  <li> clusterIndex -- the name of the cluster database index to use. 
  Default is "clusterIndex".
  <li> clusterDBType -- One of flatfile (simple cluster database) or keyfile (btree based).
  <li>  clusterType -- Type of cluster to use, either agglomerative or 
  centroid.  Centroid is agglomerative using mean which trades memory use 
  for speed of clustering. Default is centroid.
  <li>  simType -- The similarity metric to use. Default is cosine 
  similarity (COS), which is the only implemented method.
  <li>  docMode -- The scoring method to use for the
  agglomerative cluster type. The default is max (maximum).
  The choices are:
<ul>
<li> max -- Maximum score over documents in a cluster.
<li> mean -- Mean score over documents in a cluster. This is identical to the 
centroid cluster type.
<li> avg -- Average score over documents in a cluster.
<li> min -- Minimum score over documents in a cluster.
</ul>
  <li>  threshold -- Minimum score for adding a document to an existing 
  cluster. Default is 0.25.
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
    return -1;
  }
  Index *myIndex;
  try {
    myIndex  = IndexManager::openIndex(ClusterParam::databaseIndex);
  } catch (Exception &ex) {
    ex.writeMessage();
    throw Exception("Cluster", "Can't open index, check parameter index");
  }
  try {
    // construct cluster method.
    lemur::api::ClusterDB* clusterDB;
    if (ClusterParam::clusterDBType == "keyfile") {
      clusterDB = new lemur::cluster::KeyfileClusterDB(myIndex, 
                                       ClusterParam::clusterIndex,
                                       ClusterParam::threshold,
                                       ClusterParam::simType,
                                       ClusterParam::clusterType,
                                       ClusterParam::docMode);
    } else if (ClusterParam::clusterDBType == "flatfile") {
      clusterDB = new lemur::cluster::FlatFileClusterDB(myIndex, 
                                        ClusterParam::clusterIndex,
                                        ClusterParam::threshold,
                                        ClusterParam::simType,
                                        ClusterParam::clusterType,
                                        ClusterParam::docMode);
    } else {      
      // add additional db types here. Convert to Factory usage.
        cout << "Cluster database type '" << ClusterParam::clusterDBType
             << "' is not supported.\n";
        exit(1);
    }
    // crank through the collection
    double score;
    for (DOCID_T i = 1; i <= myIndex->docCount(); i++) {
      if (clusterDB->getDocClusterId(i).size() == 0) {
        // Hasn't been clustered yet.
        int myCluster = clusterDB->cluster(i, score);
        cout << myIndex->document(i) << " " << myCluster << " " 
             << score << endl;
      }
    }
    delete clusterDB;
  } catch(lemur::api::ClusterDBError x) {
    cout << "Problem with Clustering Database: " << x.what() << "\n";
  }
  delete myIndex;
  return 0;
}


