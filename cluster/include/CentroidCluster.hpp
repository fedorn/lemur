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
// init: 11/20/2002
// Centroid clustering algorithms.
#ifndef _CENTROIDCLUSTER_HPP
#define _CENTROIDCLUSTER_HPP
#include "Cluster.hpp"
namespace lemur
{
  namespace cluster 
  {    
    /// Centroid clustering of documents (agglomerative mean). Trades space for speed.
    class CentroidCluster: public Cluster
    {
    public:
      ///initialize
      CentroidCluster(int cid, const lemur::api::Index &ind, 
                      const SimilarityMethod &sim);
      /// clean up
      virtual ~CentroidCluster() {
        delete(centroid);
        delete(weightedCentroid);
      };
      /// score a document against this cluster, given the rep.
      virtual double score(const ClusterRep *rep) const;
      /// add an element
      virtual void add(const ClusterElt &elt);
      /// add a list of document ids
      virtual void add(const vector<lemur::api::DOCID_T> docids);
      /// remove the element from this cluster
      virtual void remove(const ClusterElt &elt);
      /// read a cluster in from the cluster db file.
      virtual bool read(ifstream &in);

      /// \brief Get the ClusterRep for this Cluster for scoring.
      /// The Rep will have been weighted by the SimilarityMethod.
      /// Caller responsible for deleting.
      virtual ClusterRep *getClusterRep() const;

      /// Sum of squared values in the cluster's term vector.
      virtual double sum2() const {return weightedCentroid->sum2(); }
  
    private:
      /// The centroid vector of the cluster.
      lemur::utility::FloatFreqVector *centroid;
      /// The weighted centroid vector of the cluster as a ClusterRep.
      ClusterRep *weightedCentroid;
      /// Read the centroid vector of the cluster in from disk.
      lemur::utility::FloatFreqVector* readCentroid();
      /*! \brief Add a vector to the centroid.
        @param v the vector to add
        @param first if true, initialize the centroid to v.
      */
      void updateCentroid(lemur::utility::FloatFreqVector *v, bool first);
    };
  }
}

#endif
