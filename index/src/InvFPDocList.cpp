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


/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/
#include "InvFPDocList.hpp"

InvFPDocList::InvFPDocList() : InvDocList() {
}

//  This hasn't been tested
InvFPDocList::InvFPDocList(int id, int len) : InvDocList(id, len) {
}


InvFPDocList::InvFPDocList(int id, int listlen, int* list, int fr, int* ldocid, int len) : InvDocList(id, listlen, list, fr, ldocid, len) {
}

InvFPDocList::InvFPDocList(MemCache* mc, int id, int len) : InvDocList(mc, id, len) {
}

InvFPDocList::InvFPDocList(MemCache* mc, int id, int len, int docid, int location) : InvDocList(mc, id, len) {
  if (begin)
    addLocation(docid, location);	
}

InvFPDocList::~InvFPDocList() {
}

DocInfo* InvFPDocList::nextEntry() {
  //DocInfo* dinfo;
  static InvFPDocInfo info;
  // info is stored in int* as docid freq pos1 pos2 .. 
  info.id = *iter;
  iter++;
  info.count = *iter;
  iter++;
  info.pos = iter;
  iter+=info.count;
//  dinfo = info;
  return &info;
}

void InvFPDocList::nextEntry(InvFPDocInfo* info) {
  info->id = *iter;
  iter++;
  info->count = *iter;
  iter++;
  info->pos = iter;
  iter+=info->count;
}

bool InvFPDocList::addTerm(int docid) {
  cerr << "InvFPDocList add term must have location.  Use addLocation(int, int)" << endl;
  return false;
}

bool InvFPDocList::addLocation(int docid, int location) {
  if (READ_ONLY)
    return false;
    // check that we can add at all
  if (size == 0)
    return false;

  // check to see if it's a new document
  if (docid == *lastid) {
    //get more mem if needed
    if ((end-begin+1)*intsize > size) {
      if (!getMoreMem())
        return false;
    }

    (*freq)++;
    *end = location;
    end++;
  } else {
    //get more mem if needed
    if ((end-begin+3)*intsize > size) {
      if (!getMoreMem())
        return false;
    }

    lastid = end;
    *lastid = docid;
    freq = lastid+1;
    *freq = 1;
    *(freq+1) = location;
    end = freq+2;
		df++;
  }
  return true;
}




