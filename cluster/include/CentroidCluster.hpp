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
// init: 11/20/2002
// Centroid clustering algorithms.
#ifndef _CENTROIDCLUSTER_HPP
#define _CENTROIDCLUSTER_HPP
#include "Cluster.hpp"

/// Centroid clustering of documents (agglomerative mean). Trades space for speed.
class CentroidCluster: public Cluster
{
public:
  ///initialize
  CentroidCluster(int cid, const Index &ind, const SimilarityMethod &sim);
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
  virtual void add(const vector<int> docids);
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
  FloatFreqVector *centroid;
  ClusterRep *weightedCentroid;
  FloatFreqVector* readCentroid();
  void updateCentroid(FloatFreqVector *v, bool first);
};

#endif