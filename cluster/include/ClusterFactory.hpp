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
// init: 02/05/2003
#ifndef _CLUSTERFACTORY_HPP
#define _CLUSTERFACTORY_HPP
// Add new Cluster subclasses here.
#include "ClusterMethods.hpp"

namespace lemur
{
  namespace cluster
  {
    /// Factory class for making Cluster instances.    
    class ClusterFactory {
    public:
      /// initialize the factory for the specific cluster type.
      ClusterFactory(const lemur::api::Index &ind, 
                     const SimilarityMethod &simMethod,
                     enum ClusterParam::clusterTypes clusterType = ClusterParam::CENTROID,
                     enum ClusterParam::docModes docMode = ClusterParam::DMAX);
      /// Make an instance of the appropriate cluster type.
      Cluster *allocateCluster(int clusterID = 1);
  
    private:
      const lemur::api::Index &index;
      const SimilarityMethod &sim;
      enum ClusterParam::clusterTypes cType;
      enum ClusterParam::docModes dMode;
    };
  }
}

#endif
