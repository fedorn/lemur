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

/// A package of PLSA parameter definitions and loading procedures
#include "Param.hpp"
/// General PLSA related parameters
namespace PLSAParam {
  /// @name PLSA parameters
  //@{ 
  /// database index
  static String databaseIndex;
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
    databaseIndex = ParamGetString("index","");
    numCats = ParamGetInt("numCats", 20);
    beta = ParamGetDouble("beta", 1);
    betaMin = ParamGetDouble("betaMin", 0.6);
    eta = ParamGetDouble("eta", 0.92);
    annealcue = ParamGetDouble("annealcue", 0);
    numIters = ParamGetInt("numIters", 100);
    numRestarts = ParamGetInt("numRestarts", 1);
    testPercentage = ParamGetInt("testPercentage", 10);
    String s = ParamGetString("doTrain", "true");
    doTrain = (s == "true");
  }
};
#endif
