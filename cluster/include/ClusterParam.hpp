/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _CLUSTERPARAMETER_HPP
#define _CLUSTERPARAMETER_HPP

#include "Param.hpp"
/// General cluster-related parameters
namespace ClusterParam {
  /// SimilarityMethod types.
  enum simTypes {COS=1};
  /// Concrete Cluster types.
  enum clusterTypes {AGGLOM=1, CENTROID=2};
  /// Scoring modes for AgglomerativeCluster
  enum docModes {DMAX=1, DMEAN=2, DAVE=3, DMIN=4};
  
  /// @name Cluster parameters
  //@{ 
  /// database index
  static std::string databaseIndex;
  /// cluster index
  static std::string clusterIndex;
  /// cluster database file type.
  static std::string clusterDBType;
  /// cluster type
  static enum clusterTypes clusterType;
  /// docmode
  static enum docModes docMode;
  /// threshold
  static double threshold;
  /// number of partitions
  static int numParts;
  /// max iterations
  static int maxIters;
  /// number of iters for bisecting k-means
  static int numIters;
  /// similarity method
  static enum simTypes simType;
  //@}
  /// get the parameters off of the stack.
  static void get() {
    databaseIndex = lemur::api::ParamGetString("index","");
    clusterIndex = lemur::api::ParamGetString("clusterIndex","clusterIndex");
    clusterDBType = lemur::api::ParamGetString("clusterDBType","flatfile");
    string clusterTypeString = lemur::api::ParamGetString("clusterType",
                                                          "centroid");
    if (clusterTypeString == "centroid") {
      clusterType = CENTROID;
    } else if (clusterTypeString == "agglomerative") {
      clusterType = AGGLOM;
    } else {
      cerr << "Unknown clusterType " << clusterTypeString <<
        " using centroid." << endl;
      clusterType = CENTROID;
    }
    string simTypeString = lemur::api::ParamGetString("simType","COS");
    if (simTypeString == "COS" || simTypeString == "cos") {
      simType = COS;
    } else {
      cerr << "Unrecognized simType " << simTypeString << " using cos." 
           << endl;
      simType = COS;
    }
    string docModeString = lemur::api::ParamGetString("docMode", "max");
    if (docModeString == "max") {
      docMode = DMAX;
    } else if (docModeString == "mean") {
      docMode = DMEAN;
    } else if (docModeString == "min") {
      docMode = DMIN;
    } else if (docModeString == "avg") {
      docMode = DAVE;
    } else {
      cerr << "Unrecognized docMode " << docModeString << " using max." 
           << endl;
      docMode = DMAX;
    }
    threshold = lemur::api::ParamGetDouble("threshold", 0.25);
    numParts = lemur::api::ParamGetInt("numParts", 2);
    maxIters = lemur::api::ParamGetInt("maxIters", 100);
    numIters = lemur::api::ParamGetInt("bkIters", 5);
  }
}

#endif
