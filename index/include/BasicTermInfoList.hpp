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
  virtual int count() const { return tcount;}

  virtual int id() const { return tid;}
  
  /// pretend we have position
  int position() const { return tid;}
  friend class BasicTermInfoList;
private:
  int tid, tcount;
};



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
  static BasicTermInfo info;
  info.tid =  tmpwarr[it];
  info.tcount = tmpwarr[sz+it];
  it++;
  return (&info);
}



