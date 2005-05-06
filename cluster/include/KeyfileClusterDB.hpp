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
#ifndef _KEYFILECLUSTERDB_HPP
#define _KEYFILECLUSTERDB_HPP

#include "ClusterDB.hpp"
#include "IndexedReal.hpp"
#include "Keyfile.hpp"
#include <string>
namespace lemur
{
  namespace cluster
  {
    /// Keyfile based cluster database.
    class KeyfileClusterDB : public lemur::api::ClusterDB {

    public:
      /// initialize
      KeyfileClusterDB(const lemur::api::Index *ind, 
                       const string &name,                 
                       double threshold = 0.25,
                       enum ClusterParam::simTypes simType = ClusterParam::COS,
                       enum ClusterParam::clusterTypes 
                       clusterType = ClusterParam::CENTROID,
                       enum ClusterParam::docModes docMode = ClusterParam::DMAX);

      /// clean up
      virtual ~KeyfileClusterDB();

      /// Get the Cluster for the given docId.
      virtual vector<Cluster*> getDocCluster(lemur::api::DOCID_T docId) const ;

      /// Count number of clusters
      virtual int countClusters() const {
        //clustersKey.get("NUMCLUSTERS");
        int clusterCount = 0, actual;
        clustersKey.get("NUMCLUSTERS", &clusterCount, actual, sizeof(int));
        return  clusterCount;
      }

      /// Return highest cluster ID.
      virtual int maxID() const {// get
        //clustersKey.get("MAXID");
        int clusterIDCounter = 0, actual;
        clustersKey.get("MAXID", &clusterIDCounter, actual, sizeof(int));
        return  clusterIDCounter;
      }
  

      /// Get the Cluster for the given clusterId.
      virtual Cluster *getCluster(int clusterId) const ;

      /// Get the Cluster id for the given docId.
      virtual vector<int> getDocClusterId(lemur::api::DOCID_T docId) const ;

      /// Add a document id to a cluster, given the cluster id.
      virtual int addToCluster(lemur::api::DOCID_T docId, int clusterId, 
                               double score);

      /// Add a document id to a cluster, given the cluster.
      virtual int addToCluster(lemur::api::DOCID_T docId, Cluster *cluster, 
                               double score);

      /// Remove a document id from a cluster, given the cluster id.
      virtual int removeFromCluster(lemur::api::DOCID_T docId, int clusterID);

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
      /// the document id to cluster table.
      mutable lemur::file::Keyfile doc2cluster;
      mutable lemur::file::Keyfile clustersKey;
      void init(const string &name);
      void addC2D(int cid, const vector<lemur::api::DOCID_T> &dids);
    };
  }
}

#endif
