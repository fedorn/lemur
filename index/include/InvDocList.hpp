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


#ifndef _INVDOCLIST_HPP
#define _INVDOCLIST_HPP

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/
#include <cmath>
#include "common_headers.hpp"
#include "DocInfoList.hpp"
#include "InvDocInfo.hpp"
#include "MemCache.hpp"
#include "RVLCompress.hpp"

extern "C" {
  #include <cstdio>
}

#define DEFAULT 9

class InvDocList: public DocInfoList {
public:
  InvDocList();

  /// constructor for this list using malloc for its own memory
  /// usage of InvDocList without MemCache has not been tested
  InvDocList(int id, int len);
  /// constructors for this list getting memory from a MemCache
  InvDocList(MemCache* mc, int id, int len);  
  InvDocList(MemCache* mc, int id, int len, int docid, int location);
  /// constructor for a list not needing to get any memory
  InvDocList(int id, int listlen, int* list, int fr, int* ldocid, int len);
  ~InvDocList();

  /// this is meant for use with the empty constructor
  /// this allows the DocList values to be set.  however it doesn't guarantee that 
  /// the outsider will be able to set everything properly.  thus, when this method
  /// is used, the object becomes READ_ONLY such that methods which attempt to append
  /// the contents of the list will not be able to @see setListSafe
  void setList(int id, int listlen, int* list, int fr, int* ldocid=NULL, int len=0);

  /// same as the setList above.
  /// however the READ_ONLY flag will not get set.  this method should be used only
  /// if you really know what you're doing
  void setListSafe(int id, int listlen, int* list, int fr, int* ldocid, int len);

  /// reset the list such that it points to nothing. doesn't free the memory
  /// be careful when using this to avoid memory leaks.  if you are not managing
  /// memory, you should probably use resetFree() @see resetFree
  void reset();

  /// reset the list and free the memory being used for it
  /// @see reset
  void resetFree();

  bool allocMem();
  bool hasNoMem();

  /// increase count for this docid, we don't care about location
  virtual bool addTerm(int docid);

  /// append the given list to the end of this list.  watch for overlap of lastdocid of this list and first docid of given list.
  virtual bool append(InvDocList* tail);

  virtual void startIteration();
  virtual bool hasMore();
  virtual DocInfo* nextEntry();
  virtual void nextEntry(InvDocInfo* info);

  DOCID_T curDocID() { if (lastid == NULL) return -1; return *lastid; };
  int docFreq() { return df; };
  int length() { return end-begin; };
  int termID() { return uid; };
  int termLen() { return strlength; };
  int termCTF();
  int curDocIDdiff() { return lastid-begin; };
  int curDocIDtf() { return *(lastid+1); };
  
  /// write this object in binary to the given filestream.  the stream should support binary writing.
  void binWrite(ofstream& of);

  /// read an object from the given stream into memory.  the stream should be pointing to the correct place, starting exactly where binWrite began writing.  this method should be used with the empty constructor, else watch out for mem leaks. returns whether the read was successful
  bool binRead(ifstream& inf);

  /// write this object in binary to the given filestream.  the stream should support binary writing.  RVL compression is used before writing
  void binWriteC(ofstream& of);

  /// read an object from the given stream into memory.  the stream should be pointing to the correct place, starting exactly where binWrite began writing.  this method should be used with the empty constructor, else watch out for mem leaks. returns whether the read was successful.  RVL decompression is done. 
  bool binReadC(ifstream& inf);

protected:
  /** internal method for allocating more memory to list as needed
   *  double what we had before
   */
  bool getMoreMem();
  int logb2(int num);

  /// delta encode docids from begin through end
  /// call before write
  virtual void deltaEncode();

  /// delta decode docids from begin through end
  /// call after read
  virtual void deltaDecode();

  int* begin;		// pointer to the beginning of this list
  int* lastid;	// pointer to the most recent DocID added
  int* freq;		// pointer to the frequency of the last DocID
  int* end;		  // pointer to the next free memory
  int* iter;    // pointer tells us where we are in iteration
  int  size;		// how big are we, increment in powers of 2, start at 16K
  int  intsize;	// sizeof(int) value
  int  strlength;       // the character length of our corresponding string
  TERMID_T  uid;		          // a unique ID for our string
  int  df;		          // the document frequency for current term
  MemCache* cache;      // the cache to get memory from
  bool hascache;        // remember if we have our own cache

  bool READ_ONLY;    // flag for whether this list can be added
private:
  InvDocInfo entry;
};

#endif
