/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/
#include "InvFPDocList.hpp"

InvFPDocList::InvFPDocList() {
  size = 0;
  begin = end = lastid = freq = NULL;
  uid = -1;
  df = 0;
  strlength = 0;
  intsize = sizeof(int);
  hascache = false;
}

/*
InvFPDocList::InvFPDocList(int id, int len){
  size = pow(2,DEFAULT);
  begin = (int*) malloc(size);
  lastid = begin;
  *lastid = -1;
  end = begin;
  freq = begin;
  intsize = sizeof(int);
  uid = id;
  strlength = len;
	df = 0;
  hascache = false;
}
*/

InvFPDocList::InvFPDocList(int id, int listlen, int* list, int fr, int* ldocid, int len){
  intsize = sizeof(int);
  size = listlen * intsize;
  begin = list;
  end = begin + listlen;
  uid = id;
  strlength = len;
	df = fr;
  hascache = false;
  lastid = ldocid;
  freq = lastid+1;
}

InvFPDocList::InvFPDocList(MemCache* mc, int id, int len){
  size = pow(2,DEFAULT);
  cache = mc;
  begin = cache->getMem(DEFAULT);
  if (!begin)
    size = 0;
  lastid = begin;
  if (lastid != NULL) *lastid = -1;
  end = begin;
  freq = begin;
  intsize = sizeof(int);
  hascache = true;
  uid = id;
  strlength = len;
	df = 0;
}

InvFPDocList::InvFPDocList(MemCache* mc, int id, int len, int docid, int location) {
  size = pow(2,DEFAULT);
  cache = mc;
  begin = cache->getMem(DEFAULT);
  lastid = begin;
  if (lastid != NULL) *lastid = -1;
  end = begin;
  freq = begin;
  intsize = sizeof(int);	
  hascache = true;
  df = 0;
  if (!begin) {
    size = 0;
  } else 
    addLocation(docid, location);	
  uid = id;
  strlength = len;
}

InvFPDocList::~InvFPDocList() {
/*  if (hascache) {
    int pow = logb2(size);
      cache->freeMem(begin, pow);      
  } */
}

void InvFPDocList::setList(int id, int listlen, int* list, int fr, int* ldocid, int len){
  if (hascache) {
/*    int pow = logb2(size);
      cache->freeMem(begin, pow);   
    */
  } 
  size = listlen * intsize;
  begin = list;
  end = begin + listlen;
  uid = id;
  strlength = len;
	df = fr;
  hascache = false;
  lastid = ldocid;
  freq = lastid+1;
}
  
void InvFPDocList::startIteration() {
  iter = begin;
}

bool InvFPDocList::hasMore() {
  return (iter != end);
}

DocInfo* InvFPDocList::nextEntry() {
  DocInfo* dinfo;
  InvFPDocInfo* info = new InvFPDocInfo();
  // info is stored in int* as docid freq pos1 pos2 .. 
  info->id = *iter;
  iter++;
  info->count = *iter;
  iter++;
  info->pos = iter;
  iter+=info->count;
  dinfo = info;
  return dinfo;
}

void InvFPDocList::nextEntry(InvFPDocInfo* info) {
  info->id = *iter;
  iter++;
  info->count = *iter;
  iter++;
  info->pos = iter;
  iter+=info->count;
}

bool InvFPDocList::allocMem() {
  size = pow(2,DEFAULT);

  if (hascache) {
    begin = cache->getMem(DEFAULT);
  } else {
    begin = (int*) malloc(size);
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

bool InvFPDocList::addLocation(int docid, int location) {
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

bool InvFPDocList::hasNoMem() {
  if (begin == NULL)
    return true;
  return false;
}

void InvFPDocList::reset() {
  // this is logically the right thing to do here, but we know we are going to reset
  // the cache

  if (!hascache)
    free(begin);
  /*
  if (hascache) {
    int pow = logb2(size);
    cache->freeMem(begin, pow);   
  } else {
    free(begin);
  } */
  begin = NULL;
  end = begin;
  lastid = begin;
  freq = begin;
  size = 0;
  df = 0;
}

/** THE PRIVATE STUFF **/
/** double our current mem size 
  *
  */
bool InvFPDocList::getMoreMem() {
  int diff = lastid-begin;
  int bigger = size*2;

  int pow = logb2(bigger);
  if (pow > 22)
    return false;

  if (hascache) {
    int* loc = cache->getMoreMem(pow, begin, pow-1);
    if (loc == NULL)
      return false;
    begin = loc;
  } else {
    int* old = begin;
    begin = (int*) malloc(bigger);
    memcpy(begin, old, size);
    free(old);
  }
  lastid = begin+diff;
  freq = lastid+1;
  end = (freq + *freq) +1;
  size = bigger;
  return true;
}

// a VERY VERY simplified version of log base 2 for use here only

int InvFPDocList::logb2(int num) {
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
