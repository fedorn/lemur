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
/*
  Author: dmf 11/2002
 */

#include "CosSim.hpp"
#include <cmath>

/// Cosine similarity method.
/// Precomputes the idf values for the collection.
CosSim::CosSim(const Index &dbIndex) :ind(dbIndex) {
  // pre-compute IDF values
  numTerms = dbIndex.termCountUnique();
  numDocs = dbIndex.docCount();
  idfV = new double[numTerms + 1];
  for (COUNT_T i=1; i <= numTerms; i++) {
    idfV[i] = log((numDocs + 1)/(0.5 + dbIndex.docCount(i)));
    }
  }
  
CosSim::~CosSim() {
  delete[](idfV);
}

double CosSim::similarity(const ClusterRep *vec1, const ClusterRep *vec2) 
  const {
  // Assume they are already idf weighted
  double norm = sqrt (vec1->sum2() * vec2->sum2());
  double dot_product = vec1->dotProd(vec2);
  return dot_product / ((norm == 0) ? 1 : norm);
}
