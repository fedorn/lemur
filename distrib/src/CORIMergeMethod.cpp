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

#include "CORIMergeMethod.hpp"

lemur::distrib::CORIMergeMethod::CORIMergeMethod(double constA, double constB) {
  A = constA;
  B = constB;
}

lemur::distrib::CORIMergeMethod::~CORIMergeMethod() {
}

double lemur::distrib::CORIMergeMethod::score(double dbscore, double docscore) const {
  return ( docscore + A*dbscore*docscore )/B;  
}

