/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
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
InvFPDocList::InvFPDocList(TERMID_T id, int len) : InvDocList(id, len) {
}


InvFPDocList::InvFPDocList(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid, int len) : InvDocList(id, listlen, list, fr, ldocid, len) {
}

InvFPDocList::InvFPDocList(MemCache* mc, TERMID_T id, int len) : InvDocList(mc, id, len) {
}

InvFPDocList::InvFPDocList(MemCache* mc, TERMID_T id, int len, DOCID_T docid, LOC_T location) : InvDocList(mc, id, len) {
  if (begin)
    addLocation(docid, location);	
}

InvFPDocList::InvFPDocList(LOC_T *vec) {
  READ_ONLY = false;
  hascache = false;
  LOC_Tsize = sizeof(LOC_T);
  uid = vec[0];
  df = vec[1];
  COUNT_T diff = vec[2];
  COUNT_T vecLength = vec[3];
  size = vecLength * 4;
  unsigned char* buffer = (unsigned char *) (vec + 4);
  // this should be big enough
  //  begin = (LOC_T*) malloc(s);
  // use new/delete[] so an exception will be thrown if out of memory.
  begin = new LOC_T[size/sizeof(LOC_T)];

  // decompress it
  int len = RVLCompress::decompress_ints(buffer, (int *)begin, vecLength);
  
  lastid = begin + diff;
  end = begin + len;
  freq = lastid+1;

  deltaDecode();
}

InvFPDocList::~InvFPDocList() {
}

DocInfo* InvFPDocList::nextEntry() const{
  // info is stored in LOC_T* as docid freq pos1 pos2 ..
  entry.docID(*iter);
  iter++;
  entry.termCount(*iter);
  iter++;
  entry.positions(iter);
  iter+=entry.termCount();
  return &entry;
}

void InvFPDocList::nextEntry(InvFPDocInfo* info) const{
  info->docID(*iter);
  iter++;
  info->termCount(*iter);
  iter++;
  info->positions(iter);
  iter+=info->termCount();
}

/// set element from position, returns pointer to the element
DocInfo* InvFPDocList::getElement(DocInfo* elem, POS_T position) const {
  //  InvFPDocInfo* e = dynamic_cast<InvFPDocInfo*>(elem);
  LOC_T* ip = (LOC_T*) position;
  elem->docID(*ip);
  ip++;
  elem->termCount(*ip);
  ip++;
  elem->positions(ip);
  return elem;
}
/// advance position
POS_T InvFPDocList::nextPosition(POS_T position) const {
  LOC_T* ip = (LOC_T*) position;
  return (POS_T) (ip + *(ip+1) + 2);  // ip + termcount + 2
}

bool InvFPDocList::addTerm(DOCID_T docid) {
  cerr << "InvFPDocList add term must have location.  Use addLocation(int, int)" << endl;
  return false;
}

bool InvFPDocList::append(InvDocList* par_tail) {
  if (READ_ONLY)
    return false;
  
  InvFPDocList* tail = (InvFPDocList*) par_tail;
  
  // we only want to append the actual content
  LOC_T* ptr = tail->begin;
  COUNT_T len = tail->length();
  COUNT_T diff = tail->end - tail->lastid;

  // check for memory
  while ((end-begin+len)*LOC_Tsize > size) {
    if (!getMoreMem())
      return false;
  }

  // update doc frequency
  df += tail->docFreq();
  
  // check for overlap (by 1 docid)
  // this method will mainly be used for merging lists from indexing
  // in that case, overlap of docids would only occur by 1
  if (*ptr == *lastid) {
    COUNT_T tailtf = *(ptr+1);
    // add tfs together
    *freq += tailtf;
    // advance pointer to pos list
    ptr += 2;
    memcpy(end, ptr, tailtf*LOC_Tsize);

    // doc frequency is actually one less
    df--;

    // advance pointer to next doc
    ptr += tailtf;
    len -= tailtf+2;
    end += tailtf;
  }

  // copy list over
  if (len > 0) {
    memcpy(end, ptr, len*LOC_Tsize);

    end += len;
    lastid = end-diff;
    freq = lastid+1;
  }

  return true;
}

bool InvFPDocList::addLocation(DOCID_T docid, LOC_T location) {
  if (READ_ONLY)
    return false;
    // check that we can add at all
  if (size == 0)
    return false;

  // check to see if it's a new document
  if (docid == *lastid) {
    //get more mem if needed
    if ((end-begin+1)*LOC_Tsize > size) {
      if (!getMoreMem())
        return false;
    }

    (*freq)++;
    *end = location;
    end++;
  } else {
    //get more mem if needed
    if ((end-begin+3)*LOC_Tsize > size) {
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

COUNT_T InvFPDocList::termCTF() const {
  COUNT_T ctf = 0;
  LOC_T *ptr = begin;
  while (ptr != lastid) {
    ptr++;
    ctf += (*ptr);
    ptr += (*ptr)+ 1;
  }
  ctf += (*freq);
  return ctf;
}

void InvFPDocList::deltaEncode() {
  // we will encode in place
  // go backwards starting at the last docid
  // we're counting on two always being bigger than one
  LOC_T* two = lastid;
  // to keep track of encoding positions
  LOC_T* posbeg = freq+1;
  LOC_T* posone = end-2;
  LOC_T* postwo = end-1;
  
  // our stack of ones so we can backtrack
  vector<LOC_T*> onestack;
  LOC_T* k = begin;
  while (k!=lastid) {
    LOC_T* j = k;
    onestack.push_back(j);
    //advance to freq
    k++; 
    //advance to next docid
    k += *k+1;
  }
  
  LOC_T* one;
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
  LOC_T* one = begin;
  LOC_T* two = one +*(begin+1)+2;
  
  LOC_T* posone = begin+2;
  LOC_T* postwo = begin+3;

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

LOC_T *InvFPDocList::byteVec(COUNT_T &vecLength){
  COUNT_T len= end-begin;
  COUNT_T diff = lastid-begin;
  deltaEncode();

  unsigned char* comp = new unsigned char[(len+4)*LOC_Tsize];
  vecLength = RVLCompress::compress_ints((int *)begin, comp + (4 * LOC_Tsize), len);

  LOC_T *tmp = ((LOC_T *) comp);
  tmp[0] = uid;
  tmp[1] = df;
  tmp[2] = diff;
  tmp[3] = vecLength;
  vecLength += (4 * LOC_Tsize);
  return(tmp);
}

