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


private:
  int sz;
  mutable int it;
  mutable int prevDocID;
  int *tmpdarr;
};


inline BasicDocInfoList::BasicDocInfoList(int *tmpDocArray, int size):
  tmpdarr(tmpDocArray), sz(size), it(0), prevDocID(0) 
{
}

/// This function is implemented inline, because it gets called frequently
inline DocInfo * BasicDocInfoList::nextEntry() const{
  // assert (hasMore());
  static DocInfo info;
  info.docID(tmpdarr[it]+prevDocID);
  prevDocID = info.docID();
  info.termCount(tmpdarr[sz+it]);
  it++;
  return (&info);
}

#endif


