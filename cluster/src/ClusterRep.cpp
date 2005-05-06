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
#include "ClusterRep.hpp"
#include <cmath>
lemur::cluster::ClusterRep::ClusterRep(lemur::api::DOCID_T did, const lemur::api::Index &ind) : index(ind) {
  rep = new lemur::utility::FloatFreqVector(ind, did);
}

lemur::cluster::ClusterRep::ClusterRep(lemur::api::TermInfoList *tList, const lemur::api::Index &ind) : index(ind) {
  rep = new lemur::utility::FloatFreqVector(ind, tList);
}

lemur::cluster::ClusterRep::ClusterRep(vector<lemur::api::DOCID_T> &dids, const lemur::api::Index &ind) : index(ind) {
  rep = new lemur::utility::FloatFreqVector(ind, dids);
}

lemur::cluster::ClusterRep::ClusterRep(lemur::utility::FloatFreqVector *v, const lemur::api::Index &ind) : 
  index(ind) {
  rep = new lemur::utility::FloatFreqVector(v);
}

lemur::cluster::ClusterRep::ClusterRep(const ClusterRep *old): index(old->index) {
  rep = new lemur::utility::FloatFreqVector(old->rep);
}

lemur::cluster::ClusterRep::~ClusterRep() {
  delete(rep);
}

void lemur::cluster::ClusterRep::normalize() {
  double norm = sqrt(rep->sum2());
  rep->weigh(1/norm);
}

