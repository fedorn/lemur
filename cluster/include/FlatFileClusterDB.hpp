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

// Peter Amstutz
// init: 12/17/2002
#ifndef _FLATFILECLUSTERDB_HPP
#define _FLATFILECLUSTERDB_HPP

#include "ClusterDB.hpp"
#include "IndexedReal.hpp"
#include <string>
/*!
  Writes one file, <i>name</i>.cl, the cluster
  database. 
*/
class FlatFileClusterDB : public ClusterDB {

public:
  /// initialize
  FlatFileClusterDB(const Index *ind, 
		    const string &name, 		   
		    double threshold = 0.25,
		    enum ClusterParam::simTypes simType = ClusterParam::COS,
		    enum ClusterParam::clusterTypes 
		      clusterType = ClusterParam::CENTROID,
		    enum ClusterParam::docModes docMode = ClusterParam::DMAX);

  /// clean up
  virtual ~FlatFileClusterDB();

  /// Get the Cluster for the given docId.
  virtual vector<Cluster*> getDocCluster(int docId) const;

  /// Count number of clusters
  virtual int countClusters() const {return clusterCount - 1;}

  /// Return highest cluster ID.
  virtual int maxID() const{return clusterIdCounter - 1;}

  /// Get the Cluster for the given clusterId.
  virtual Cluster *getCluster(int clusterId) const;

  /// Get the Cluster id for the given docId.
  virtual vector<int> getDocClusterId(int docId) const;

  /// Add a document id to a cluster, given the cluster id.
  virtual int addToCluster(int docId, int clusterId, double score);

  /// Add a document id to a cluster, given the cluster.
  virtual int addToCluster(int docId, Cluster *cluster, double score);

  /// Remove a document id from a cluster, given the cluster id.
  virtual int removeFromCluster(int docId, int clusterID);

  /// Delete a cluster entirely
  virtual int deleteCluster(int clusterID);

  /// Delete a cluster entirely
  virtual int deleteCluster(Cluster * cluster);

  /// split cluster -- using Cluster::split.
  virtual vector<int> splitCluster(int cid, int num = 2);

  /// merge two clusters.
  virtual int mergeClusters(int cid1, int cid2);

protected:
  /// Factory method for creating Cluster objects.
  virtual Cluster * newCluster();

private:
  int clusterCount;
  int clusterIdCounter;
  string clusterDB;
  /// the document id to cluster id table.
  int *doc2cluster;
  void readClusterDB();
  void writeClusterDB();
  void init(const string &name);
};
#endif