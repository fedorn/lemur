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

bool InvFPDocList::append(InvDocList* par_tail) {
  if (READ_ONLY)
    return false;
  
  InvFPDocList* tail = (InvFPDocList*) par_tail;
  
  // we only want to append the actual content
  int* ptr = tail->begin;
  int len = tail->length();
  int diff = tail->end - tail->lastid;

  // check for memory
  while ((end-begin+len)*intsize > size) {
    if (!getMoreMem())
      return false;
  }

  // update doc frequency
  df += tail->docFreq();
  
  // check for overlap (by 1 docid)
  // this method will mainly be used for merging lists from indexing
  // in that case, overlap of docids would only occur by 1
  if (*ptr == *lastid) {
    int tailtf = *(ptr+1);
    // add tfs together
    *freq += tailtf;
    // advance pointer to pos list
    ptr += 2;
    memcpy(end, ptr, tailtf*intsize);

    // doc frequency is actually one less
    df--;

    // advance pointer to next doc
    ptr += tailtf;
    len -= tailtf+2;
    end += tailtf;
  }

  // copy list over
  if (len > 0) {
    memcpy(end, ptr, len*intsize);

    end += len;
    lastid = end-diff;
    freq = lastid+1;
  }

  return true;
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


void InvFPDocList::deltaEncode() {
  // we will encode in place
  // go backwards starting at the last docid
  // we're counting on two always being bigger than one
  int* two = lastid;
  // to keep track of encoding positions
  int* posbeg = freq+1;
  int* posone = end-2;
  int* postwo = end-1;
  
  // our stack of ones so we can backtrack
  vector<int*> onestack;
  int* k = begin;
  while (k!=lastid) {
    int* j = k;
    onestack.push_back(j);
    //advance to freq
    k++; 
    //advance to next docid
    k += *k+1;
  }
  
  int* one;
  while (two != begin) {
    one = onestack.back();
    onestack.pop_back();

    // do the positions
    while (postwo != posbeg) {
      *postwo = *postwo - *posone;
      postwo = posone;
      posone--;
    }
    posbeg = one+2;
    postwo = two-1;
    posone = two-2;
    
    *two = *two-*one;
    two = one;
  }
  // do the positions for the first doc
  while (postwo != posbeg) {
    *postwo = *postwo - *posone;
    postwo = posone;
    posone--;
  }
}

void InvFPDocList::deltaDecode() {
  // we will decode in place
  // start at the begining
  int* one = begin;
  int* two = one +*(begin+1)+2;
  
  int* posone = begin+2;
  int* postwo = begin+3;

  while (one != lastid) {
    // do the positions
    while (postwo != two) {
      *postwo = *posone + *postwo;
      posone = postwo;
      postwo++;
    }
    *two = *two + *one;
    one = two;
    two = one + *(one+1)+2;
    posone = one+2;
    postwo = one+3;
  }
  //do positions for the last one
  while (postwo != two) {
    *postwo = *posone + *postwo;
    posone = postwo;
    postwo++;
  }
}
