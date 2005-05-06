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

#include "DocScore.hpp"

lemur::distrib::DocScoreVector::ScoreAscending lemur::distrib::DocScoreVector::ascendOrder;
lemur::distrib::DocScoreVector::ScoreDescending lemur::distrib::DocScoreVector::descendOrder;

void lemur::distrib::DocScoreVector::Sort(bool descending)
{
  if (descending) {
    sort(this->begin(), this->end(), descendOrder);
  } else {
    sort(this->begin(), this->end(), ascendOrder);
  }
}


void lemur::distrib::DocScoreVector::PushValue(const string &str, double value)
{
  DocScore entry;
  entry.id = str;
  entry.val = value;
  push_back(entry);
}

lemur::distrib::DocScoreVector::~DocScoreVector() {
#if 0
  // superceded by <string>
  //free memory from our strdup in PushValue
  // parent destructor should take care of rest
  for (int i=0;i<size();i++) {
    if ((*this)[i].id) 
      free((*this)[i].id);
  }
#endif
}


