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
// Agglomerative clustering algorithms.
#ifndef _AGGLOMCLUSTER_HPP
#define _AGGLOMCLUSTER_HPP
#include "Cluster.hpp"

namespace lemur
{
  /// Document clustering components.
  namespace cluster
  {
    
    /// K nearest neighbors clustering of documents. d_ave, d_min, d_max, d_mean scoring methods.
    class AgglomCluster: public Cluster
    {
    public:
      ///initialize
      AgglomCluster(int cid, const lemur::api::Index &ind, 
                    const SimilarityMethod &sim, 
                    enum ClusterParam::docModes mode): Cluster(cid, ind, sim), 
                                                       docmode(mode) {}
      /// clean up
      virtual ~AgglomCluster() {};
      /// score a document against this cluster, given the rep.
      virtual double score(const ClusterRep *rep) const;
      /// \brief Get the ClusterRep for this Cluster for scoring.
      /// The Rep will have been weighted by the SimilarityMethod.
      /// Caller responsible for deleting.
      virtual ClusterRep *getClusterRep() const;
      /// Sum of squared values in the cluster's term vector.
      virtual double sum2() const;
  
    private:
      /// Scoring mode to use.
      enum ClusterParam::docModes docmode;
      /// Score the rep using min.
      double score_min(const ClusterRep *rep) const;
      /// Score the rep using max.
      double score_max(const ClusterRep *rep) const;
      /// Score the rep using mean.
      double score_mean(const ClusterRep *rep) const;
      /// Score the rep using average.
      double score_ave(const ClusterRep *rep) const;
    };
  }
}

#endif
