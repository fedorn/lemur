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

// David Fisher
// init: 11/18/2002
// Agglomerative clustering algorithms.
#include "AgglomCluster.hpp"

ClusterRep *AgglomCluster::getClusterRep() const {
  vector <int> docids = getDocIds();
  ClusterRep *elt = new ClusterRep(docids, ind);
  elt->weigh(1.0/docids.size());
  similarity.weigh(elt);
  return elt;
}

double AgglomCluster::sum2() const {
  ClusterRep *me = getClusterRep();
  double value = me->sum2();
  delete(me);
  return value;
}

double AgglomCluster::score_max(const ClusterRep *rep) const {
  ClusterRep *elt = NULL;
  double max_score = 0, s = 0;
  vector <int> docids = getDocIds();
  int sz = docids.size();
  for (int i = 0; i < sz; i++) {
    elt = new ClusterRep(docids[i], ind);
    similarity.weigh(elt);
    s = similarity.similarity(rep, elt);
    if (s > max_score) {
      max_score = s;
    }
    delete(elt);
  }
  return max_score;
}

double AgglomCluster::score_min(const ClusterRep *rep) const {
  ClusterRep *elt = NULL;
  double min_score = 1.1, s = 0;
  if (size == 0) return s;
  vector <int> docids = getDocIds();
  for (int i = 0; i < docids.size(); i++) {
    elt = new ClusterRep(docids[i], ind);
    similarity.weigh(elt);
    s = similarity.similarity(rep, elt);
    if (s < min_score) {
      min_score = s;
    }
    delete(elt);
  }
  return min_score;
}

double AgglomCluster::score_mean(const ClusterRep *rep) const {
  double s = 0;
  ClusterRep *elt = getClusterRep();
  s = similarity.similarity(rep, elt);
  delete(elt);
  return s;
}

double AgglomCluster::score_ave(const ClusterRep *rep) const {
  ClusterRep *elt = NULL;
  double score = 0;
  if (size == 0) return score;
  vector <int> docids = getDocIds();
  for (int i = 0; i < docids.size(); i++) {
    elt = new ClusterRep(docids[i], ind);
    similarity.weigh(elt);
    score += similarity.similarity(rep, elt);
    delete(elt);
  }
  // average over docs only.
  return score/docids.size();
}

double AgglomCluster::score(const ClusterRep *rep) const {
  switch (docmode) {
  case ClusterParam::DMAX:
    return score_max(rep);
  case ClusterParam::DMIN:
    return score_min(rep);
  case ClusterParam::DAVE:
    return score_ave(rep);
  case ClusterParam::DMEAN:
    return score_mean(rep);
  default:
    return 0;
  }
}
