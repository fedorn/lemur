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


#ifndef _INDEXEDREAL_HPP
#define _INDEXEDREAL_HPP

/// A list of indexed real numbers (similar to IndexProb)

/*!

 CLASSES: IndexedReal, IndexedRealVector

 IndexedReal: a real number with an integer index
 IndexedRealVector: A vector of IndexedReal
 
 Possible use: (1) sorting doc scores; (2) representing query vector;
   (3) sorting probabilities

*/

// CLASSES: IndexedReal, IndexedRealVector (C. Zhai, 12/1/2000)

#include "common_headers.hpp"
#include <algorithm>

struct IndexedReal {
  int ind;
  double val;
};


class IndexedRealVector : public vector<IndexedReal> {
public:
  
  IndexedRealVector() : vector<IndexedReal>() {}
  IndexedRealVector(int size) : vector<IndexedReal>(size) {}
  virtual ~IndexedRealVector() {}
  virtual iterator FindByIndex(int index);

  /// increase the value for "index", add the entry if not existing.
  // return true iff the entry already exists.
  virtual bool IncreaseValueFor(int index, double value);

  /// push a value 
  virtual void PushValue(int index, double value);

  /// sort all the values, default is descending order
  virtual void Sort(bool descending = true);
private:

  /// Function object types, 
  /// defines how to compare IndexedReal objects (for sorting/searching)

  struct IndexedRealAscending {  
    bool operator()(const IndexedReal & a, const IndexedReal & b) {
      return a.val < b.val;  // based on the real value
    }
  };

  struct IndexedRealDescending {  
    bool operator()(const IndexedReal & a, const IndexedReal & b) {
      return a.val > b.val;  // based on the real value
    }
  };

  IndexedRealAscending ascendOrder;
  IndexedRealDescending descendOrder;

};


//==================== inline implementations ====================

inline void IndexedRealVector::Sort(bool descending)
{
  if (descending) {
    sort(this->begin(), this->end(), descendOrder);
  } else {
    sort(this->begin(), this->end(), ascendOrder);
  }
}


inline IndexedRealVector::iterator IndexedRealVector::FindByIndex(int index)
{
  iterator it = begin();
  while (it != end() && (*it).ind != index) {
    it++;
  }
  return  it;
}

inline bool IndexedRealVector::IncreaseValueFor(int index, double value)
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


inline void IndexedRealVector::PushValue(int index, double value)
{
  IndexedReal entry;
  entry.ind = index;
  entry.val = value;
  push_back(entry);
}

#endif //_INDEXEDREAL_HPP


  


