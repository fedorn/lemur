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


#include "TermInfoList.hpp"

/// Basic TermInfoList

class BasicTermInfoList : public TermInfoList {
public:
  BasicTermInfoList(int *tmpWordArray, int size);
  
  virtual ~BasicTermInfoList() {}
  virtual void startIteration() const{
    it = 0;
  }

  virtual bool hasMore() const{ return (it<sz); }

  virtual TermInfo *nextEntry() const;

protected:
  // Helper functions for iterator, subclasses should override
  /// set element from position, returns pointer to the element
  virtual TermInfo* getElement(TermInfo* elem, POS_T position) const;
  /// position at beginning of list
  virtual POS_T beginPosition() const { return (POS_T) 0; }
  /// position at end of list
  virtual POS_T endPosition() const { return (POS_T) sz; }
  /// advance position
  virtual POS_T nextPosition(POS_T position) const;

private:
  int sz;
  mutable int it;
  int *tmpwarr;
};

