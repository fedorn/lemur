#ifndef _INVFPDOCLIST_HPP
#define _INVFPDOCLIST_HPP

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/
#include <math.h>
#include "DocInfoList.hpp"
#include "InvFPDocInfo.hpp"
#include "MemCache.hpp"

#define DEFAULT 9

class InvFPDocList: public DocInfoList {
public:
  /// constructor for this list using malloc for its own memory
  InvFPDocList(int id, int len);
  /// constructors for this list getting memory from a MemCache
  InvFPDocList(MemCache* mc, int id, int len);  
  InvFPDocList(MemCache* mc, int id, int len, int docid, int location);
  /// constructor for a list not needing to get any memory
  InvFPDocList(int id, int listlen, int* list, int fr, int* ldocid, int len);
  ~InvFPDocList();

  void setList(int id, int listlen, int* list, int fr, int* ldocid, int len);
  void reset();
  bool allocMem();
  bool hasNoMem();
  bool addLocation(int docid, LOC_T location);

  void startIteration();
  bool hasMore();
  DocInfo* nextEntry();
  void nextEntry(InvFPDocInfo* info);

  DOCID_T curDocID() { return *lastid; };
  int docFreq() { return df; };
  int length() { return end-begin; };
  int termID() { return uid; };
  int termLen() { return strlength; };

private:
  /** internal method for allocating more memory to list as needed
   *  double what we had before
   */
  bool getMoreMem();
  int logb2(int num);

  int* begin;		// pointer to the beginning of this list
  int* lastid;	// pointer to the most recent DocID added
  int* freq;		// pointer to the frequency of the last DocID
  int* end;		  // pointer to the next free memory
  int* iter;    // pointer tells us where we are in iteration
  int  size;		// how big are we, increment in powers of 2, start at 16K
  int  intsize;	// sizeof(int) value
  int  strlength;       // the character length of our corresponding string
  int  uid;		          // a unique ID for our string
  int  df;		          // the document frequency for current term
  MemCache* cache;      // the cache to get memory from
  bool hascache;        // remember if we have our own cache
};

#endif
