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
#include "InvDocList.hpp"

InvDocList::InvDocList() {
  READ_ONLY = true;
  size = 0;
  begin = end = lastid = freq = NULL;
  uid = -1;
  df = 0;
  strlength = 0;
  LOC_Tsize = sizeof(LOC_T);
  hascache = false;
}

//  This hasn't been tested
InvDocList::InvDocList(TERMID_T id, int len){
  READ_ONLY = false;
  size = (int) pow(2.0,DEFAULT);
  //  begin = (int*) malloc(size);
  // use new/delete[] so an exception will be thrown if out of memory.
  begin = new LOC_T[size/sizeof(LOC_T)];
  lastid = begin;
  *lastid = -1;
  end = begin;
  freq = begin;
  LOC_Tsize = sizeof(LOC_T);
  uid = id;
  strlength = len;
  df = 0;
  hascache = false;
}


InvDocList::InvDocList(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid, int len){
  //we don't own the memory.. 
  READ_ONLY = true;
  LOC_Tsize = sizeof(LOC_T);
  size = listlen * LOC_Tsize;
  begin = list;
  end = begin + listlen;
  uid = id;
  strlength = len;
  df = fr;
  hascache = false;
  lastid = ldocid;
  freq = lastid+1;
}

InvDocList::InvDocList(MemCache* mc, TERMID_T id, int len){
  READ_ONLY = false;
  size = (int) pow(2.0,DEFAULT);
  cache = mc;
  begin = (LOC_T*)cache->getMem(DEFAULT);
  if (!begin)
    size = 0;
  lastid = begin;
  if (lastid != NULL) *lastid = -1;
  end = begin;
  freq = begin;
  LOC_Tsize = sizeof(LOC_T);
  hascache = true;
  uid = id;
  strlength = len;
  df = 0;
}

InvDocList::~InvDocList() {
/*  if (hascache) {
    int pow = logb2(size);
      cache->freeMem(begin, pow);      
  } */
/*
  if ((begin != NULL) && (!hascache) && (!READ_ONLY))
    free(begin);
*/
  if ((!hascache) && (!READ_ONLY))
    delete[](begin);

}

void InvDocList::setList(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid, int len){
  READ_ONLY = true;
/*
  if (hascache) {
    int pow = logb2(size);
    cache->freeMem(begin, pow);     
  } else if (begin != NULL) {
    free(begin);
  }
  */
  size = listlen * LOC_Tsize;
  begin = list;
  end = begin + listlen;
  uid = id;
  strlength = len;
	df = fr;
  hascache = false;
  lastid = ldocid;
  freq = lastid+1;
}

void InvDocList::setListSafe(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid, int len){
  if (hascache) {
/*    int pow = logb2(size);
      cache->freeMem(begin, pow);
    */
  }
  size = listlen * LOC_Tsize;
  begin = list;
  end = begin + listlen;
  uid = id;
  strlength = len;
	df = fr;
  hascache = false;
  lastid = ldocid;
  freq = lastid+1;
}

void InvDocList::startIteration() const{
  iter = begin;
}

bool InvDocList::hasMore() const{
  return (iter != end);
}

DocInfo* InvDocList::nextEntry() const{
  // use the attribute entry.
  //  static InvDocInfo info;
  // info is stored in LOC_T* as docid freq .. ..
  entry.docID(*iter);
  iter++;
  entry.termCount(*iter);
  iter++;
  return &entry;
}

void InvDocList::nextEntry(DocInfo* info) const{
  info->docID(*iter);
  iter++;
  info->termCount(*iter);
  iter++;
}

/// set element from position, returns pointer to the element
DocInfo* InvDocList::getElement(DocInfo* elem, POS_T position) const {
  // info is stored in LOC_T* as docid freq .. ..
  LOC_T* ip = (LOC_T*) position;
  elem->docID(*ip);
  ip++;
  elem->termCount(*ip);
  return elem;
}
/// advance position
POS_T InvDocList::nextPosition(POS_T position) const {
  // info is stored in LOC_T* as docid freq .. ..
  return (POS_T) (((LOC_T*) position) + 2);
}

bool InvDocList::allocMem() {
  if (READ_ONLY)
    return false;

  size = (int) pow(2.0,DEFAULT);

  if (hascache) {
    begin = (LOC_T *)cache->getMem(DEFAULT);
  } else {
    //    begin = (TERMID_T*) malloc(size);
    // use new/delete[] so an exception will be thrown if out of memory.
    begin = new LOC_T[size/sizeof(LOC_T)];
  }
  lastid = begin;
  if (lastid != NULL) *lastid = -1;
  end = begin;
  freq = begin;
  if (!begin) {
    size = 0;
    return false;
  }
  return true;
}

bool InvDocList::addTerm(DOCID_T docid) {
  if (READ_ONLY)
    return false;
    // check that we can add at all
  if (size == 0)
    return false;

  // check to see if it's a new document
  if (docid == *lastid) {
    (*freq)++;
  } else {
    //get more mem if needed
    if ((end-begin+2)*LOC_Tsize > size) {
      if (!getMoreMem())
        return false;
    }

    lastid = end;
    *lastid = docid;
    freq = lastid+1;
    *freq = 1;
    end = freq+1;
    df++;
  }
  return true;
}

bool InvDocList::append(InvDocList* tail) {
  if (READ_ONLY)
    return false;

  // we only want to append the actual content
  LOC_T *ptr = tail->begin;
  int len = tail->length();

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
    // add tfs together
    *freq += *(ptr+1);
    // advance pointer to next doc
    ptr += 2;
    len -= 2;
    // doc frequency is actually one less
    df--;
  }

  // copy list over
  if (len > 0) {
    memcpy(end, ptr, len*LOC_Tsize);

    end += len;
    lastid = end-2;
    freq = end-1;
  }

  return true;
}

COUNT_T InvDocList::termCTF() const{
  COUNT_T ctf = 0;
  LOC_T *start = begin;
  while (start != lastid){
    start++;
    ctf += *start;
    start++;
  }
  ctf += (*freq);
  return ctf;  
}

bool InvDocList::hasNoMem() {
  if (begin == NULL)
    return true;
  return false;
}

void InvDocList::reset() {
  /*
  if (hascache) {
    int pow = logb2(size);
    cache->freeMem(begin, pow);
  } else if (begin != NULL) {
    free(begin);
  } */
  begin = NULL;
  end = begin;
  lastid = begin;
  freq = begin;
  size = 0;
  df = 0;
}

void InvDocList::resetFree() {
  // free the memory
  if (hascache) {
    int pow = logb2(size);
    cache->freeMem((int *)begin, pow);   
  } else if (begin != NULL) {
    //    free(begin);
    delete[](begin);
  } 

  begin = end = lastid = freq = NULL;
  size = 0;
  df = 0;
  uid = -1;
}

void InvDocList::binWrite(ofstream& of) {
  COUNT_T len= end-begin;
  COUNT_T diff = lastid-begin;
  of.write((const char*) &uid, LOC_Tsize);
  of.write((const char*) &df, LOC_Tsize);
  of.write((const char*) &diff, LOC_Tsize);
  of.write((const char*) &len, LOC_Tsize);
  of.write((const char*) begin, LOC_Tsize*len);
}

bool InvDocList::binRead(ifstream& inf) {
  if (inf.eof())
    return false;
  int diff;

  inf.read((char*) &uid, sizeof(TERMID_T));
  if (!(inf.gcount() == sizeof(TERMID_T)))
    return false;

  inf.read((char*) &df, LOC_Tsize);
  if (!inf.gcount() == LOC_Tsize)
    return false;

  inf.read((char*) &diff, LOC_Tsize);
  if (!inf.gcount() == LOC_Tsize)
    return false;

  inf.read((char*) &size, LOC_Tsize);
  if (!inf.gcount() == LOC_Tsize)
    return false;

  int s = sizeof(LOC_T)*size;
  //  begin = (LOC_T*) malloc(s);
  // use new/delete[] so an exception will be thrown if out of memory.
  begin = new LOC_T[s/sizeof(LOC_T)];

  inf.read((char*) begin, s);
  if (!inf.gcount() == s) {
    resetFree();
    return false;
  }

  lastid = begin + diff;
  end = begin + size;
  freq = lastid+1;
  READ_ONLY = false;
  return true;
}

void InvDocList::binWriteC(ofstream& of) {
  COUNT_T len= end-begin;
  COUNT_T diff = lastid-begin;
  of.write((const char*) &uid, sizeof(TERMID_T));
  of.write((const char*) &df, LOC_Tsize);
  of.write((const char*) &diff, LOC_Tsize);

  deltaEncode();

  // compress it
  // it's ok to make comp the same size.  the compressed will be smaller
  //  unsigned char* comp = (unsigned char*) malloc(len*LOC_Tsize);
  // use new/delete so an exception will be thrown if out of memory
  unsigned char* comp = new unsigned char[len*LOC_Tsize];
  int compbyte = RVLCompress::compress_ints((int *)begin, comp, len);
  
  // write out the compressed bits
  of.write((const char*) &compbyte, LOC_Tsize);
  of.write((const char*) comp, compbyte);

  //  of.write((const char*) &len, LOC_Tsize);
  //of.write((const char*) begin, sizeof(LOC_T)*len);
  //  free(comp);
  delete[](comp);
}

bool InvDocList::binReadC(ifstream& inf) {
  if (inf.eof())
    return false;
  int diff;

  inf.read((char*) &uid, sizeof(TERMID_T));
  if (!(inf.gcount() == sizeof(TERMID_T)))
    return false;

  inf.read((char*) &df, LOC_Tsize);
  if (!inf.gcount() == LOC_Tsize)
    return false;

  inf.read((char*) &diff, LOC_Tsize);
  if (!inf.gcount() == LOC_Tsize)
    return false;

  inf.read((char*) &size, LOC_Tsize);
  if (!inf.gcount() == LOC_Tsize)
    return false;

  //  unsigned char* buffer = (unsigned char*) malloc(size);
  // use new/delete[] so an exception will be thrown if out of memory.
  unsigned char* buffer = new unsigned char[size];
  inf.read((char*) buffer, size);
  if (!inf.gcount() == size) {
    resetFree();
    return false;
  }

  // this should be big enough
  //  begin = (LOC_T*) malloc(size*4);  
  // use new/delete[] so an exception will be thrown if out of memory.
  begin = new LOC_T[(size*4)/sizeof(LOC_T)];  

  // decompress it
  int len = RVLCompress::decompress_ints(buffer, (int *)begin, size);

  size = size*4;

  if (len * LOC_Tsize > size)
    cerr << "RVLDecompress in DocList buffer overrun!" << endl;

  lastid = begin + diff;
  end = begin + len;
  freq = lastid+1;

  deltaDecode();

  READ_ONLY = false;
  //  free(buffer);
  delete[](buffer);
  return true;
}

/** THE PRIVATE STUFF **/
/** double our current mem size 
  *
  */
void InvDocList::deltaEncode() {
  // we will encode in place
  // go backwards starting at the last docid
  // we're counting on two always being bigger than one
  LOC_T* two = lastid;
  LOC_T* one = lastid-2;

  while (two != begin) {
    *two = *two-*one;
    two = one;
    one -= 2;
  }
}

void InvDocList::deltaDecode() {
  // we will decode in place
  // start at the begining
  LOC_T* one = begin;
  LOC_T* two = begin+2;

  while (one != lastid) {
    *two = *two + *one;
    one = two;
    two += 2;
  }
}


bool InvDocList::getMoreMem() {
  int ldiff = lastid-begin;
  int enddiff = end-begin;
  int bigger = size*2;

  if (hascache) {
    int pow = logb2(bigger);
    if (pow > 22)
      return false;

    LOC_T* loc = (LOC_T *)cache->getMoreMem(pow, (int *)begin, pow-1);
    if (loc == NULL)
      return false;
    begin = loc;
  } else {
    LOC_T* old = begin;
    //    begin = (int*) malloc(bigger);
    // use new/delete[] so an exception will be thrown if out of memory
    begin = new LOC_T[bigger/sizeof(LOC_T)];
    memcpy(begin, old, size);
    //free(old);
    delete[](old);
  }
  lastid = begin+ldiff;
  freq = lastid+1;
  end = begin+enddiff;
  size = bigger;
  return true;
}

// a VERY VERY simplified version of log base 2 for use here only

int InvDocList::logb2(int num) {
  // we know that the answer should be between 5 and 22
  switch (num) {
  case(32):
    return 5;
  case(64):
    return 6;
  case(128):
    return 7;
  case(256):
    return 8;
  case(512):
    return 9;
  case(1024):
    return 10;
  case(2048):
    return 11;
  case(4096):
    return 12;
  case(8192):
    return 13;
  case(16384):
    return 14;
  case(32768):
    return 15;
  case(65536):
    return 16;
  case(131072):
    return 17;
  case(262144):
    return 18;
  case(524288):
    return 19;
  case(1048576):
    return 20;
  case(2097152):
    return 21;
  case(4194304):
    return 22;
  default:
    fprintf(stderr, "getMoreMem: trying to get bigger list than we expected.\n");
    return 23; 
  }
}


