 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _INVFPDOCLIST_HPP
#define _INVFPDOCLIST_HPP

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/
#include <math.h>
#include <iostream>
#include <fstream>
#include "DocInfoList.hpp"
#include "InvFPDocInfo.hpp"
#include "MemCache.hpp"

extern "C" {
  #include <stdio.h>
}

#define DEFAULT 9

class InvFPDocList: public DocInfoList {
public:
  InvFPDocList();

  /// constructor for this list using malloc for its own memory
  /// usage of InvFPDocList without MemCache has not been tested
  InvFPDocList(int id, int len);
  /// constructors for this list getting memory from a MemCache
  InvFPDocList(MemCache* mc, int id, int len);  
  InvFPDocList(MemCache* mc, int id, int len, int docid, int location);
  /// constructor for a list not needing to get any memory
  InvFPDocList(int id, int listlen, int* list, int fr, int* ldocid, int len);
  ~InvFPDocList();

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
  bool addLocation(int docid, LOC_T location);

  void startIteration();
  bool hasMore();
  DocInfo* nextEntry();
  void nextEntry(InvFPDocInfo* info);

  DOCID_T curDocID() { if (lastid == NULL) return -1; return *lastid; };
  int docFreq() { return df; };
  int length() { return end-begin; };
  int termID() { return uid; };
  int termLen() { return strlength; };
  int curDocIDdiff() { return lastid-begin; };
  int curDocIDtf() { return *(lastid+1); };

  /// write this object in binary to the given filestream.  the stream should support
  /// binary writing.
  void binWrite(ofstream& of);

  /// read an object from the given stream into memory.  the stream should be pointing
  /// to the correct place, starting exactly where binWrite began writing.
  /// this method should be used with the empty constructor, else watch out for mem leaks.
  /// returns whether the read was successful
  bool binRead(ifstream& inf);

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
  TERMID_T  uid;		          // a unique ID for our string
  int  df;		          // the document frequency for current term
  MemCache* cache;      // the cache to get memory from
  bool hascache;        // remember if we have our own cache

  bool READ_ONLY;    // flag for whether this list can be added
};

#endif
