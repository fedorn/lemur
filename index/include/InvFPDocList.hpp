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


#ifndef _INVFPDOCLIST_HPP
#define _INVFPDOCLIST_HPP

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 * tnt 06/2002 - subclass from InvDocList
 *
 *========================================================================*/
#include <cmath>
#include "common_headers.hpp"
#include "InvDocList.hpp"
#include "InvFPDocInfo.hpp"
#include "MemCache.hpp"

extern "C" {
  #include <cstdio>
}

#define DEFAULT 9

class InvFPDocList: public InvDocList {
public:
  InvFPDocList();
  /// copy from byteVector
  InvFPDocList(int *vec);

  /// constructor for this list using malloc for its own memory
  /// usage of InvFPDocList without MemCache has not been tested
  InvFPDocList(int id, int len);
  /// constructors for this list getting memory from a MemCache
  InvFPDocList(MemCache* mc, int id, int len);  
  InvFPDocList(MemCache* mc, int id, int len, int docid, int location);
  /// constructor for a list not needing to get any memory
  InvFPDocList(int id, int listlen, int* list, int fr, int* ldocid, int len);
  ~InvFPDocList();

  bool addTerm(int docid);
  bool addLocation(int docid, LOC_T location);
  bool append(InvDocList* tail);

  virtual DocInfo* nextEntry();
  virtual void nextEntry(InvFPDocInfo* info);
  virtual int termCTF();
  int *byteVec(int &len);

protected:
  /// delta encode docids and positions from begin through end
  /// call before write
  virtual void deltaEncode();

  /// delta decode docids and positions from begin through end
  /// call after read
  virtual void deltaDecode();
  private:
  InvFPDocInfo entry;
};

#endif
