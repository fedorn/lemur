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
#include "ClusterFactory.hpp"

ClusterFactory::ClusterFactory(const Index &ind, 
			       const SimilarityMethod &simMethod,
			       enum ClusterParam::clusterTypes clusterType,
			       enum ClusterParam::docModes docMode) :
  index(ind), sim(simMethod), cType(clusterType), dMode(docMode) {
}

Cluster *ClusterFactory::allocateCluster(int clusterID) {
    switch (cType) {
    case ClusterParam::CENTROID:
        return new CentroidCluster(clusterID, index, sim);
    case ClusterParam::AGGLOM:
        return new AgglomCluster(clusterID, index, sim, dMode);
    default:
        cerr << "Unknown cluster type " << cType << endl;
        return NULL;
    }
}

