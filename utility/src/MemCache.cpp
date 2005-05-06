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

#include <math.h>
#include "MemCache.hpp"

lemur::utility::MemCache::MemCache(int cachesize) {
  size = cachesize;
  begin = (int*) malloc(size);
  end = begin;
  intsize = sizeof(int);
  ourmem = true;
}

lemur::utility::MemCache::MemCache(int* cache, int cachesize) {
  size = cachesize;
  begin = cache;
  end = begin;
  intsize = sizeof(int);
  ourmem = false;
}

lemur::utility::MemCache::MemCache() {
  size = 0;
}

lemur::utility::MemCache::~MemCache() {
  if (ourmem)
    free(begin);
}

int* lemur::utility::MemCache::getMem(int chunksize) {
  int bytesize = (int) pow(2.0, chunksize);
  int s = bytesize/intsize;
  int *retval = NULL;

  // if we have from free, use that
  retval = getFromFree(chunksize);
  if (retval != NULL) {
    return retval;
  }
  // else get it from the end
  // make sure we still have enough mem
  if (((end - begin)*intsize) + bytesize > size) {
    return NULL;
  }
  // give a chunk from the end, update end
  retval = end;
  end += s;
  
  return retval;
}

int* lemur::utility::MemCache::getMoreMem(int newsize, int* location, int oldsize) {
  if (oldsize >= newsize)
    return NULL;
  
  if ((oldsize < MINPOW) || (oldsize > MAXPOW))
    return NULL;

  if ((newsize < MINPOW) || (newsize > MAXPOW))
    return NULL;

  //get the big chunk
  int* retval = getMem(newsize);
  if (retval == NULL)
    return NULL;

  //do the copy
  memcpy(retval, location, (int) pow(2.0, oldsize));
        
  //store this free spot
  //  *location = oldsize;
  //  freelist.push_back(location);
  freelist[oldsize-MINPOW].push_back(location);

  return retval;
}

void lemur::utility::MemCache::freeMem(int* location, int memsize) {
  // add this memory location to the free list
  //*location = memsize;
  //freelist.push_back(location);
  freelist[memsize-MINPOW].push_back(location);
}


void lemur::utility::MemCache::flushMem(){
  //reinitialize
  end = begin;

  //clear out new list
  for (int i=0; i<NLISTS; i++) {
    freelist[i].clear();
  }
}

const int* lemur::utility::MemCache::getBegin() {
  return begin;
}

const int* lemur::utility::MemCache::getEnd() {
  return end;
}

/** PRIVATE **/
int* lemur::utility::MemCache::getFromFree(int csize) {
  if (freelist[csize-MINPOW].empty())
    return NULL;

  int* retval = freelist[csize-MINPOW].back();
  freelist[csize-MINPOW].pop_back();

  return retval;

  /*
    if (freelist.empty()) 
    return NULL;
  
    for (vector<int*>::iterator i=freelist.begin(); i!=freelist.end(); i++) {
    int avail = **i;
    if (avail == csize) {
    int* retval = *i;
    freelist.erase(i);
    return retval;
    } else if (avail > csize) {
    int* retval = *i;
    int diff = avail-csize;
    *i += csize/intsize;
    **i = diff;
    return retval;
    }
    }
  
    return NULL;
  */
}
