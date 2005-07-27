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

lemur::index::InvFPDocList::InvFPDocList() : InvDocList() {
}

//  This hasn't been tested
lemur::index::InvFPDocList::InvFPDocList(lemur::api::TERMID_T id, int len) : InvDocList(id, len) {
}


lemur::index::InvFPDocList::InvFPDocList(lemur::api::TERMID_T id, int listlen, lemur::api::LOC_T* list, int fr, lemur::api::DOCID_T* ldocid, int len) : InvDocList(id, listlen, list, fr, ldocid, len) {
}

lemur::index::InvFPDocList::InvFPDocList(lemur::utility::MemCache* mc, lemur::api::TERMID_T id, int len) : InvDocList(mc, id, len) {
}

lemur::index::InvFPDocList::InvFPDocList(lemur::utility::MemCache* mc, lemur::api::TERMID_T id, int len, lemur::api::DOCID_T docid, lemur::api::LOC_T location) : InvDocList(mc, id, len) {
  if (begin)
    addLocation(docid, location);       
}

lemur::index::InvFPDocList::InvFPDocList(lemur::api::LOC_T *vec) {
  READ_ONLY = false;
  hascache = false;
  LOC_Tsize = sizeof(lemur::api::LOC_T);
  uid = vec[0];
  df = vec[1];
  lemur::api::COUNT_T diff = vec[2];
  lemur::api::COUNT_T vecLength = vec[3];
  size = vecLength * 4;
  unsigned char* buffer = (unsigned char *) (vec + 4);
  // this should be big enough
  //  begin = (LOC_T*) malloc(s);
  // use new/delete[] so an exception will be thrown if out of memory.
  begin = new lemur::api::LOC_T[size/sizeof(lemur::api::LOC_T)];

  // decompress it
  int len = lemur::utility::RVLCompress::decompress_ints(buffer, (int *)begin, vecLength);
  
  lastid = begin + diff;
  end = begin + len;
  freq = lastid+1;

  deltaDecode();
}

lemur::index::InvFPDocList::~InvFPDocList() {
}

lemur::api::DocInfo* lemur::index::InvFPDocList::nextEntry() const{
  // info is stored in LOC_T* as docid freq pos1 pos2 ..
  entry.docID(*iter);
  iter++;
  entry.termCount(*iter);
  iter++;
  entry.positions(iter);
  iter+=entry.termCount();
  return &entry;
}

void lemur::index::InvFPDocList::nextEntry(InvFPDocInfo* info) const{
  info->docID(*iter);
  iter++;
  info->termCount(*iter);
  iter++;
  info->positions(iter);
  iter+=info->termCount();
}

/// set element from position, returns pointer to the element
lemur::api::DocInfo* lemur::index::InvFPDocList::getElement(lemur::api::DocInfo* elem, lemur::api::POS_T position) const {
  //  InvFPDocInfo* e = dynamic_cast<InvFPDocInfo*>(elem);
  lemur::api::LOC_T* ip = begin + position;
  elem->docID(*ip);
  ip++;
  elem->termCount(*ip);
  ip++;
  elem->positions(ip);
  return elem;
}
/// advance position
lemur::api::POS_T lemur::index::InvFPDocList::nextPosition(lemur::api::POS_T position) const {
  lemur::api::LOC_T* ip = begin + position;
  return (lemur::api::POS_T) (position + *(ip+1) + 2);  // ip + termcount + 2
}

bool lemur::index::InvFPDocList::addTerm(lemur::api::DOCID_T docid) {
  cerr << "InvFPDocList add term must have location.  Use addLocation(int, int)" << endl;
  return false;
}

bool lemur::index::InvFPDocList::append(InvDocList* par_tail) {
  if (READ_ONLY)
    return false;
  
  InvFPDocList* tail = (InvFPDocList*) par_tail;
  
  // we only want to append the actual content
  lemur::api::LOC_T* ptr = tail->begin;
  lemur::api::COUNT_T len = tail->length();
  lemur::api::COUNT_T diff = tail->end - tail->lastid;

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
    lemur::api::COUNT_T tailtf = *(ptr+1);
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

bool lemur::index::InvFPDocList::addLocation(lemur::api::DOCID_T docid, lemur::api::LOC_T location) {
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

lemur::api::COUNT_T lemur::index::InvFPDocList::termCTF() const {
  lemur::api::COUNT_T ctf = 0;
  lemur::api::LOC_T *ptr = begin;
  while (ptr != lastid) {
    ptr++;
    ctf += (*ptr);
    ptr += (*ptr)+ 1;
  }
  ctf += (*freq);
  return ctf;
}

void lemur::index::InvFPDocList::deltaEncode() {
  // we will encode in place
  // go backwards starting at the last docid
  // we're counting on two always being bigger than one
  lemur::api::LOC_T* two = lastid;
  // to keep track of encoding positions
  lemur::api::LOC_T* posbeg = freq+1;
  lemur::api::LOC_T* posone = end-2;
  lemur::api::LOC_T* postwo = end-1;
  
  // our stack of ones so we can backtrack
  vector<lemur::api::LOC_T*> onestack;
  lemur::api::LOC_T* k = begin;
  while (k!=lastid) {
    lemur::api::LOC_T* j = k;
    onestack.push_back(j);
    //advance to freq
    k++; 
    //advance to next docid
    k += *k+1;
  }
  
  lemur::api::LOC_T* one;
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

void lemur::index::InvFPDocList::deltaDecode() {
  // we will decode in place
  // start at the begining
  lemur::api::LOC_T* one = begin;
  lemur::api::LOC_T* two = one +*(begin+1)+2;
  
  lemur::api::LOC_T* posone = begin+2;
  lemur::api::LOC_T* postwo = begin+3;

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

lemur::api::LOC_T *lemur::index::InvFPDocList::byteVec(lemur::api::COUNT_T &vecLength){
  lemur::api::COUNT_T len= end-begin;
  lemur::api::COUNT_T diff = lastid-begin;
  deltaEncode();

  unsigned char* comp = new unsigned char[(len+4)*LOC_Tsize];
  vecLength = lemur::utility::RVLCompress::compress_ints((int *)begin, comp + (4 * LOC_Tsize), len);

  lemur::api::LOC_T *tmp = ((lemur::api::LOC_T *) comp);
  tmp[0] = uid;
  tmp[1] = df;
  tmp[2] = diff;
  tmp[3] = vecLength;
  vecLength += (4 * LOC_Tsize);
  return(tmp);
}

