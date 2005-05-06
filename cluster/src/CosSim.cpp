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
/*
  Author: dmf 11/2002
*/

#include "CosSim.hpp"
#include <cmath>

/// Cosine similarity method.
/// Precomputes the idf values for the collection.
lemur::cluster::CosSim::CosSim(const lemur::api::Index &dbIndex) :ind(dbIndex) {
  // pre-compute IDF values
  numTerms = dbIndex.termCountUnique();
  numDocs = dbIndex.docCount();
  idfV = new double[numTerms + 1];
  for (lemur::api::COUNT_T i=1; i <= numTerms; i++) {
    idfV[i] = log((numDocs + 1)/(0.5 + dbIndex.docCount(i)));
  }
}
  
lemur::cluster::CosSim::~CosSim() {
  delete[](idfV);
}

double lemur::cluster::CosSim::similarity(const lemur::cluster::ClusterRep *vec1, const lemur::cluster::ClusterRep *vec2) 
  const {
  // Assume they are already idf weighted
  double norm = sqrt (vec1->sum2() * vec2->sum2());
  double dot_product = vec1->dotProd(vec2);
  return dot_product / ((norm == 0) ? 1 : norm);
}
