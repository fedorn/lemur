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
#include "ClusterRep.hpp"
#include <cmath>
ClusterRep::ClusterRep(int did, const Index &ind) : index(ind) {
  rep = new FloatFreqVector(ind, did);
}

ClusterRep::ClusterRep(TermInfoList *tList, const Index &ind) : index(ind) {
  rep = new FloatFreqVector(ind, tList);
}

ClusterRep::ClusterRep(vector<int> &dids, const Index &ind) : index(ind) {
  rep = new FloatFreqVector(ind, dids);
}

ClusterRep::ClusterRep(FloatFreqVector *v, const Index &ind) : 
index(ind) {
  rep = new FloatFreqVector(v);
}

ClusterRep::ClusterRep(const ClusterRep *old): index(old->index) {
  rep = new FloatFreqVector(old->rep);
}

ClusterRep::~ClusterRep() {
  delete(rep);
}

void ClusterRep::normalize() {
  double norm = sqrt(rep->sum2());
  rep->weigh(1/norm);
}

