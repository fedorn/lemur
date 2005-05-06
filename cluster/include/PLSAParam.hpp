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

#ifndef _PLSAPARAMETER_HPP
#define _PLSAPARAMETER_HPP

#include "Param.hpp"
/// General PLSA related parameters
namespace PLSAParam {
  /// @name PLSA parameters
  //@{ 
  /// database index
  static std::string databaseIndex;
  /// number of categories
  static int numCats;
  /// Beta for TEM
  static double beta, betaMin;
  /// eta for TEM (beta = eta * beta;)
  static double eta;
  /// annealcue value (delta)
  static double annealcue;
  /// How many iterations
  static int numIters;
  /// How many restarts
  static int numRestarts;
  /// Test percentage for splitting (10%)
  static int testPercentage;
  /// Are we training?
  static bool doTrain;
  //@}

  static void get() {
    databaseIndex = lemur::api::ParamGetString("index","");
    numCats = lemur::api::ParamGetInt("numCats", 20);
    beta = lemur::api::ParamGetDouble("beta", 1);
    betaMin = lemur::api::ParamGetDouble("betaMin", 0.6);
    eta = lemur::api::ParamGetDouble("eta", 0.92);
    annealcue = lemur::api::ParamGetDouble("annealcue", 0);
    numIters = lemur::api::ParamGetInt("numIters", 100);
    numRestarts = lemur::api::ParamGetInt("numRestarts", 1);
    testPercentage = lemur::api::ParamGetInt("testPercentage", 10);
    std::string s = lemur::api::ParamGetString("doTrain", "true");
    doTrain = (s == "true");
  }
};
#endif
