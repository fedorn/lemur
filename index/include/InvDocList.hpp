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


#ifndef _INVDOCLIST_HPP
#define _INVDOCLIST_HPP

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/
#include <cmath>
#include "InvFPTypes.hpp"
#include "common_headers.hpp"
#include "DocInfoList.hpp"
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
  InvDocList(TERMID_T id, int len);
  /// constructors for this list getting memory from a MemCache
  InvDocList(MemCache* mc, TERMID_T id, int len);  
  InvDocList(MemCache* mc, TERMID_T id, int len, DOCID_T docid, LOC_T location);
  /// constructor for a list not needing to get any memory
  InvDocList(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid, int len);
  ~InvDocList();

  /// this is meant for use with the empty constructor
  /// this allows the DocList values to be set.  however it doesn't guarantee that 
  /// the outsider will be able to set everything properly.  thus, when this method
  /// is used, the object becomes READ_ONLY such that methods which attempt to append
  /// the contents of the list will not be able to @see setListSafe
  void setList(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid=NULL, int len=0);

  /// same as the setList above.
  /// however the READ_ONLY flag will not get set.  this method should be used only
  /// if you really know what you're doing
  void setListSafe(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid, int len);

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
  virtual bool addTerm(DOCID_T docid);

  /// append the given list to the end of this list.  watch for overlap of lastdocid of this list and first docid of given list.
  virtual bool append(InvDocList* tail);

  virtual void startIteration() const;
  virtual bool hasMore() const;
  virtual DocInfo* nextEntry() const;
  virtual void nextEntry(DocInfo* info) const;

  DOCID_T curDocID() const{ if (lastid == NULL) return -1; return *lastid; };
  COUNT_T docFreq() const{ return df; };
  int length() const{ return end-begin; };
  TERMID_T termID() const{ return uid; };
  int termLen() const{ return strlength; };
  virtual COUNT_T termCTF() const;
  int curDocIDdiff() const{ return lastid-begin; };
  int curDocIDtf() const{ return *(lastid+1); };
  int memorySize() const{ return size; };

  /// write this object in binary to the given filestream.  the stream should support binary writing.
  void binWrite(ofstream& of);

  /// read an object from the given stream into memory.  the stream should be pointing to the correct place, starting exactly where binWrite began writing.  this method should be used with the empty constructor, else watch out for mem leaks. returns whether the read was successful
  bool binRead(ifstream& inf);

  /// write this object in binary to the given filestream.  the stream should support binary writing.  RVL compression is used before writing
  void binWriteC(ofstream& of);

  /// read an object from the given stream into memory.  the stream should be pointing to the correct place, starting exactly where binWrite began writing.  this method should be used with the empty constructor, else watch out for mem leaks. returns whether the read was successful.  RVL decompression is done.
  bool binReadC(ifstream& inf);

protected:
  // Helper functions for iterator, subclasses should override
  /// set element from position, returns pointer to the element
  virtual DocInfo* getElement(DocInfo* elem, POS_T position) const;
  /// position at beginning of list
  virtual POS_T beginPosition() const { return (POS_T) begin; }
  /// position at end of list
  virtual POS_T endPosition() const { return (POS_T) end; }
  /// advance position
  virtual POS_T nextPosition(POS_T position) const;

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

  // Use LOC_T* for TERMID/DOCID/COUNT/LOC.
  LOC_T* begin;		// pointer to the beginning of this list
  LOC_T* lastid;	// pointer to the most recent DocID added
  LOC_T* freq;		// pointer to the frequency of the last DocID
  LOC_T * end;		  // pointer to the next free memory
  mutable LOC_T* iter;    // pointer tells us where we are in iteration
  int  size;		// how big are we, increment in powers of 2, start at 16K
  int  LOC_Tsize;	// sizeof(LOC_T) value
  int  strlength;       // the character length of our corresponding string
  TERMID_T  uid;		          // a unique ID for our string
  COUNT_T  df;		          // the document frequency for current term
  MemCache* cache;      // the cache to get memory from
  bool hascache;        // remember if we have our own cache

  bool READ_ONLY;    // flag for whether this list can be added
private:
  mutable DocInfo entry;
};

#endif
