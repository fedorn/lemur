#ifndef _MEMCACHE_HPP
#define _MEMCACHE_HPP

/** Memory managment Cache */
/** This is a cache for managing memory.  You allocate a chunk of memory
and request pointers to smaller pieces of it. You can also trade memory
for bigger pieces.  It will copy stuff over for you.  This cache will only
hand out memory in chunk sizes of powers of 2 from 2^10 through 2^22.
*/

/*
 *
 * NAME DATE - COMMENTS
 * tnt 11/2000 - created
 * tnt 03/2001 - mudge to lemur toolkit standards
 *
 * =====================================================================*/
#include <stdlib.h>
#include <math.h>
#include "vector.hpp"

#define MINPOW 5
#define MAXPOW 22
#define NLISTS MAXPOW-MINPOW+1

class MemCache {
public:
  /** constructor
   * @param cachesize how much memory to allocate this cache in bytes
   */
  MemCache(int cachesize);

  /** default constructor that doesn't do much */
  MemCache();

  /** destructor
   * free the memory we allocated
   */
  ~MemCache();

  /** 
   * @return a pointer to some memory location of size chunksize
   * @param chunksize how much memory we need in bytes as a power of 2, 
   */
  int* getMem(int chunksize);

  /** returns pointer to a new location of newsize containing
   * all the elements of the old location
   *
   * @return NULL if we can't get more memory, or if we are requesting a newsize that is not bigger than the oldsize
   */	
  int* getMoreMem(int newsize, int* location, int oldsize);

  /** "free" the memory at this pointer
   * this cache will remember it in the free list
   * this does not mean delete the memory
   * @param location beginning of location of memory
   * @param memsize how much to free at this location
   */
  void freeMem(int* location, int memsize);

  /**
   * reinitializes
   */
  void flushMem();

  int* getBegin();
  int* getEnd();

private:
  /** returns pointer to memory holes if available
   * returns NULL if nothing is available
   *
   * @param int csize size of memory we're requesting
   */
  int* getFromFree(int csize);

  int* begin;		/// point to the start of our cache
  int  size;		/// the size of our cache	
  int* end;	        /// index of where free memory begins
  int  intsize;	        /// sizeof(int) value
  //vector<int*> freelist;	/// a list of free memory holes
  vector<int*> freelist[NLISTS]; /// array of lists of free memory 
};

#endif
