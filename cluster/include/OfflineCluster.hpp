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
#ifndef _OFFLINECLUSTER_HPP
#define _OFFLINECLUSTER_HPP
#include <set>
#include "common_headers.hpp"
#include "Index.hpp"
#include "ClusterParam.hpp"
#include "ClusterFactory.hpp"
#include "SimFactory.hpp"
namespace lemur 
{
  namespace cluster
  {
    
    /// Offline clustering algorithms.
    class OfflineCluster
    {
    public:
      /// initialize the cluster methods
      OfflineCluster(const lemur::api::Index &ind, 
                     enum ClusterParam::simTypes simType = ClusterParam::COS,
                     enum ClusterParam::clusterTypes clusterType = ClusterParam::CENTROID,
                     enum ClusterParam::docModes docMode = ClusterParam::DMAX);

      /// clean up
      ~OfflineCluster();

      /// Cluster a set of documents into numParts partitions (default 2).
      /// k-means caller responsible for deleting contents of return vector.
      vector<Cluster*> *kMeans(vector<lemur::api::DOCID_T> docIds, 
                               int numParts = 2, int maxIters = 100);

      /// k-means caller responsible for deleting contents of  return vector.
      vector<Cluster*> *kMeans(Cluster *cluster, int numParts = 2, 
                               int maxIters = 100);

      /// bisecting k-means caller responsible for deleting contents of
      /// return vector.
      vector<Cluster*> *bisecting_kMeans(vector<lemur::api::DOCID_T> docIds, 
                                         int numParts = 2, 
                                         int numIters = 5, int maxIters = 100);

    private:
      /// Similarity Method to use
      const SimilarityMethod *sim;
      /// Cluster factory
      ClusterFactory *factory;
      /// Database containing the collection to operate on.
      const lemur::api::Index &index;
      /// Are two sets of clusters equal?
      bool compareClusterSets(Cluster **, Cluster **, int n);
      /// Choose num seeds randomly from docIds.
      vector <lemur::api::DOCID_T> selectSeeds(vector<lemur::api::DOCID_T> docIds, int num);
      /// Choose largest cluster from working to split.
      Cluster *chooseSplit(vector<Cluster *> *working);  
      /// Score sum of within cluster similarity over a set of clusters
      double scoreSet(vector<Cluster *> *working);
    };
  }
}

#endif
