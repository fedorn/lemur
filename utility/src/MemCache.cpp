#include "MemCache.hpp"

MemCache::MemCache(int cachesize) {
  size = cachesize;
  begin = (int*) malloc(size);
  end = begin;
  intsize = sizeof(int);
}

MemCache::MemCache() {
  size = 0;
}

MemCache::~MemCache() {
  free(begin);
}

int* MemCache::getMem(int chunksize) {
  int bytesize = pow(2,chunksize);
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

int* MemCache::getMoreMem(int newsize, int* location, int oldsize) {
  if (oldsize >= newsize)
    return NULL;
  
  if ((oldsize < MIN) || (oldsize > MAX))
    return NULL;

  if ((newsize < MIN) || (newsize > MAX))
    return NULL;

  //get the big chunk
  int* retval = getMem(newsize);
  if (retval == NULL)
    return NULL;

  //do the copy
  memcpy(retval, location, pow(2,oldsize));
	
  //store this free spot
//  *location = oldsize;
//  freelist.push_back(location);
	freelist[oldsize-MIN].push_back(location);

  return retval;
}

void MemCache::freeMem(int* location, int memsize) {
  // add this memory location to the free list
  //*location = memsize;
  //freelist.push_back(location);
  freelist[memsize-MIN].push_back(location);
}


void MemCache::flushMem(){
  //reinitialize
  end = begin;

  //clear out new list
  for (int i=0; i<NLISTS; i++) {
    freelist[i].clear();
  }
}

int* MemCache::getBegin() {
  return begin;
}

int* MemCache::getEnd() {
  return end;
}

/** PRIVATE **/
int* MemCache::getFromFree(int csize) {
  if (freelist[csize-MIN].empty())
    return NULL;

  int* retval = freelist[csize-MIN].back();
  freelist[csize-MIN].pop_back();

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
