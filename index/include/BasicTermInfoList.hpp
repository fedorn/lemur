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

/// BasicTermInfo

class BasicTermInfo : public TermInfo {
public:
  BasicTermInfo() {}
  BasicTermInfo( int termID, int termCount) : 
    tid(termID), tcount(termCount) {}
  virtual int count() { return tcount;}

  virtual int id() { return tid;}
  
  /// pretend we have position
  int position() { return tid;}
  friend class BasicTermInfoList;
private:
  int tid, tcount;
};



/// Basic TermInfoList

class BasicTermInfoList : public TermInfoList {
public:
  BasicTermInfoList(int *tmpWordArray, int size);
  
  virtual ~BasicTermInfoList() {}
  virtual void startIteration() {
    it = 0;
  }

  virtual bool hasMore() { return (it<sz); }

  virtual TermInfo *nextEntry();

private:
  int sz, it;
  int *tmpwarr;
};


inline BasicTermInfoList::BasicTermInfoList(int *tmpWordArray, int size):
  tmpwarr(tmpWordArray), sz(size), it(0) 
{
}

inline TermInfo * BasicTermInfoList::nextEntry() {
  // assert (hasMore());
  static BasicTermInfo info;
  info.tid =  tmpwarr[it];
  info.tcount = tmpwarr[sz+it];
  it++;
  return (&info);
}



