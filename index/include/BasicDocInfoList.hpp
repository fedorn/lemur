 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _BASICDOCINFOLIST_HPP
#define _BASICDOCINFOLIST_HPP

#include "DocInfoList.hpp"


/// BasicDocInfo

class BasicDocInfo : public DocInfo {
public:
  BasicDocInfo() {}
  BasicDocInfo( int docID, int termCount) : 
    id(docID), count(termCount) {}
  virtual int termCount() { return count;}

  virtual int docID() { return id;}
  friend class BasicDocInfoList;
private:
  int id, count;
};


/// Implementation of DocInfoList for BasicIndex

class BasicDocInfoList : public DocInfoList {
public:
  BasicDocInfoList(int *tmpDocArray, int size);

  virtual ~BasicDocInfoList() {}
  virtual void startIteration() {
    it = 0;
    prevDocID = 0;
  }

  virtual bool hasMore() { return (it<sz); }

  virtual DocInfo * nextEntry();


private:
  int sz, it, prevDocID;
  int *tmpdarr;
};


inline BasicDocInfoList::BasicDocInfoList(int *tmpDocArray, int size):
  tmpdarr(tmpDocArray), sz(size), it(0), prevDocID(0) 
{
}

inline DocInfo * BasicDocInfoList::nextEntry() {
  // assert (hasMore());
  static BasicDocInfo info;
  info.id =  tmpdarr[it]+prevDocID;
  prevDocID = info.id;
  info.count = tmpdarr[sz+it];
  it++;
  return (&info);
}

#endif


