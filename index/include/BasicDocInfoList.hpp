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


#ifndef _BASICDOCINFOLIST_HPP
#define _BASICDOCINFOLIST_HPP

#include "DocInfoList.hpp"

/// Implementation of DocInfoList for BasicIndex

class BasicDocInfoList : public DocInfoList {
public:
  BasicDocInfoList(int *tmpDocArray, int size);

  virtual ~BasicDocInfoList() {}
  virtual void startIteration() const {
    it = 0;
    prevDocID = 0;
  }

  virtual bool hasMore() const { return (it<sz); }

  virtual DocInfo * nextEntry() const;

protected:
  // Helper functions for iterator, subclasses should override
  /// set element from position, returns pointer to the element
  virtual DocInfo* getElement(DocInfo* elem, POS_T position) const;
  /// position at beginning of list
  virtual POS_T beginPosition() const { return (POS_T) 0; }
  /// position at end of list
  virtual POS_T endPosition() const { return (POS_T) sz; }
  /// advance position
  virtual POS_T nextPosition(POS_T position) const;

private:
  int sz;
  mutable int it;
  mutable int prevDocID;
  int *tmpdarr;
};

#endif

