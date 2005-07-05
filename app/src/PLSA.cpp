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
/// Probabilistic Latent Semantic Analysis

/*! \page PLSA Probabilistic Latent Semantic Analysis
  This application will either perform PLSA on a 
  collection, building three probability tables: P(z), P(d|z), and P(w|z)
  where z in Z are the latent variables (categories), d in D are the 
  documents in the collection, and w in W are the terms in the vocabulary
  over the collection, or open those tables and read them into memory
  to illustrate their potential use.

  The parameter <tt>doTrain (true|false)</tt> determines whether the tables
  are constructed or read. The default value is <tt>true</tt>.

  The other parameters accepted by PLSA are:
  <ul>
  <li> index -- the index to use. Default is none.
  <li>  numCats -- the number of latent variables (categories) to use.
  Default is 20.
  <li>  beta -- The value of beta for Tempered EM (TEM). Default is 1.
  <li>  betaMin -- The minimum value for beta, TEM iterations stop when
  beta falls below this value. Default is 0.6.
  <li>  eta -- Multiplier to scale beta before beginning a new set of TEM 
  iterations. Must be less than 1. Default is 0.92.
  <li>  annealcue -- Minimum allowed difference between likelihood in 
  consecutive iterations. If the difference is less than this, beta is
  updated. Default is 0.
  <li>  numIters -- Maximum number of iterations to perform. Default is 100.
  <li>  numRestarts -- Number of times to recompute with different random
  seeds. Default is 1.
  <li>  testPercentage -- Percentage of events (d,w) to hold out for 
  validation.
  <li>  doTrain -- whether to construct the probability tables or read 
  them in. Default is true.
  </ul>
*/
/*
  author: dmf
 */

#include "IndexManager.hpp"
#include "PLSA.hpp"
#include "PLSAParam.hpp"

using namespace lemur::api;
using namespace lemur::cluster;

// get application parameters
void GetAppParam() {
  PLSAParam::get();
}

int AppMain(int argc, char * argv[]) {
  Index *myIndex;
  myIndex  = IndexManager::openIndex(PLSAParam::databaseIndex);
  lemur::cluster::PLSA *plsa;
  int numDocs = myIndex->docCount() + 1;
  int numCats = PLSAParam::numCats;
  int numRestart = PLSAParam::numRestarts;
  int numIter = PLSAParam::numIters;
  double beta = PLSAParam::beta;
  double betaMin = PLSAParam::betaMin;
  double eta = PLSAParam::eta;
  double annealcue = PLSAParam::annealcue;
  int testPercentage = PLSAParam::testPercentage;
  bool doTrain = PLSAParam::doTrain;
  int i, d, w, z;  

  if (doTrain) {
    plsa = new lemur::cluster::PLSA(*myIndex, testPercentage, numCats, 
                                    numIter, numRestart, 
                                    beta, betaMin, annealcue, eta);
    plsa->iterateWithRestarts();
  } else {
    plsa = new lemur::cluster::PLSA(*myIndex);
    if (!plsa->readArrays()) {
      std::cerr << "Failed to load probability tables." << std::endl;
      exit (1);
    }
    int numTerms = plsa->numWords();
    numDocs = plsa->numDocs();
    numCats = plsa->numCats();
    double lim = 1.0/numTerms;
    cout << "P(d,w):" << endl;
    for(d = 1; d < numDocs; d++) { // DOC
      cout << myIndex->document(d) << ": ";
      for (w = 1; w < numTerms; w++) {
        double prob = plsa->getProb(d,w);
        if (prob > lim) cout << myIndex->term(w) << "(" <<  prob << ") ";
      }
      cout << endl;
    }
    double **p_d_z = plsa->get_p_d_z();
    double **p_w_z = plsa->get_p_w_z();
    double *p_z = plsa->get_p_z();
    cout << "P(w|z):" << endl;
    for(w = 1; w < numTerms; w++) { // TERM
      cout << myIndex->term(w) << ": ";
      for (z = 0; z < numCats; z++) {
        cout << z << "(" << p_w_z[w][z] << ") ";
      }
      cout << endl;
    }
    cout << "P(d|z):" << endl;
    for(d = 1; d < numDocs; d++) { // DOC
      cout << myIndex->document(d) << ": ";
      for (z = 0; z < numCats; z++) {
        cout << z << "(" << p_d_z[d][z] << ") ";
      }
      cout << endl;
    }
    cout << "P(z):" << endl;
    for(i = 0; i < numCats; i++) {
      cout << "cat " << i << ": " << p_z[i] << endl;
    }
  }
  delete(plsa);
  delete(myIndex);
  return 0;
}


