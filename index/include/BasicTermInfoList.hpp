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

private:
  int sz;
  mutable int it;
  int *tmpwarr;
};


inline BasicTermInfoList::BasicTermInfoList(int *tmpWordArray, int size):
  tmpwarr(tmpWordArray), sz(size), it(0) 
{
}

/// This function is implemented inline, because it gets called frequently
inline TermInfo * BasicTermInfoList::nextEntry() const{
  // assert (hasMore());
  static TermInfo info;
  info.termID(tmpwarr[it]);
  info.count(tmpwarr[sz+it]);
  it++;
  return (&info);
}



