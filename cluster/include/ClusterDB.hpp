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

// David Fisher, Peter Amstutz
// init: 11/18/2002
#ifndef _CLUSTERDB_HPP
#define _CLUSTERDB_HPP
#include <stdexcept>
#include "common_headers.hpp"
#include "Index.hpp"
#include "IndexedReal.hpp"
#include "ClusterParam.hpp"
#include "ClusterFactory.hpp"
#include "SimFactory.hpp"
#include "ThresholdFcn.hpp"

/// Exception to throw for clustering errors.
class ClusterDBError : public runtime_error
{
public:
    ClusterDBError(const string& s) : runtime_error(s) { };
};

/// Abstract interface for clustering databases.

class ClusterDB
{
public:
  /// initialize the cluster method
  ClusterDB(const Index *ind, 
	    double threshold = 0.25,
	    enum ClusterParam::simTypes simType = ClusterParam::COS,
	    enum ClusterParam::clusterTypes clusterType=ClusterParam::CENTROID,
	    enum ClusterParam::docModes docMode = ClusterParam::DMAX);

  /// clean up
  virtual ~ClusterDB();

  /// Return number of clusters
  virtual int countClusters() const = 0;

  /// Return highest cluster ID.
  virtual int maxID() const = 0;

  /// Get the Cluster for the given docId.
  virtual vector<Cluster*> getDocCluster(DOCID_T docId) const = 0;

  /// Get the Cluster for the given clusterId.
  virtual Cluster *getCluster(int clusterId) const = 0;

  /// Get the Cluster id for the given docId.
  virtual vector<int> getDocClusterId(DOCID_T docId) const = 0;

  /// Add a document id to a cluster, given the cluster id.
  virtual int addToCluster(DOCID_T docId, int clusterId, double score) = 0;

  /// Add a document id to a cluster, given the cluster.
  virtual int addToCluster(DOCID_T docId, Cluster *cluster, double score) = 0;

  /// Remove a document id from a cluster, given the cluster id.
  virtual int removeFromCluster(DOCID_T docId, int clusterID) = 0;
  /// Delete a cluster entirely
  virtual int deleteCluster(int clusterID) = 0;

  /// Delete a cluster entirely
  virtual int deleteCluster (Cluster *target) = 0;

  /// Add a cluster to the DB (renumbers the cluster id). Returns new id.
  int addCluster(Cluster *oldCluster);

  /// split cluster using Cluster::split
  virtual vector<int> splitCluster(int cid, int num = 2) = 0;

  /// merge two clusters.
  virtual int mergeClusters(int cid1, int cid2) = 0;

  /// pretty print all clusters to the standard output.
  virtual void printClusters() const;

  /// Assign a document to a cluster.
  virtual int cluster(DOCID_T docId);

  /// Assign a document to a cluster, returning score.
  virtual int cluster(DOCID_T docId, double &finalScore);

  /// Get the top N keywords for a cluster.
  virtual string getKeyWords(int cid, int numTerms = 10) const;

protected:
  /// Database containing the collection to operate on.
  const Index *index;
  /// Number of documents in the database, reduces calls to db->docCount().
  int numDocs;
  /// Number of terms in the database.
  int numTerms;
  /// threshold for YES/NO decisions
  double threshold; // fix this.
  /// the cluster database.
  mutable vector<Cluster *> clusters;
  /// Similarity method to use.
  const SimilarityMethod *sim;
  /// Cluster factory
  ClusterFactory *factory;
  /// subclass specific cluster intitialization.
  virtual Cluster * newCluster() = 0;
  /// Uses ClusterFactory to create Cluster objects.
  Cluster* allocateCluster(int clusterID) const;
  /// Threshold function for adaptive thresholding.
  ThresholdFcn *thresh;
};
#endif
