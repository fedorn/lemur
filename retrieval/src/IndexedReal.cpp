/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#include "IndexedReal.hpp"



void IndexedRealVector::Sort(bool descending)
{
  if (descending) {
    sort(this->begin(), this->end(), descendOrder);
  } else {
    sort(this->begin(), this->end(), ascendOrder);
  }
}


IndexedRealVector::iterator IndexedRealVector::FindByIndex(int index)
{
  iterator it = begin();
  while (it != end() && (*it).ind != index) {
    it++;
  }
  return  it;
}

bool IndexedRealVector::IncreaseValueFor(int index, double value)
{
  iterator it = FindByIndex(index);
  if (it == end()) {
    IndexedReal entry;
    entry.ind = index;
    entry.val = value;
    push_back(entry);
    return false;
  } else {
    (*it).val += value;
    return true;
  }
}


void IndexedRealVector::PushValue(int index, double value)
{
  IndexedReal entry;
  entry.ind = index;
  entry.val = value;
  push_back(entry);
}


