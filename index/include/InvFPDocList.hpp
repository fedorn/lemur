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
  InvFPDocList(LOC_T *vec);

  /// constructor for this list using malloc for its own memory
  /// usage of InvFPDocList without MemCache has not been tested
  InvFPDocList(TERMID_T id, int len);
  /// constructors for this list getting memory from a MemCache
  InvFPDocList(MemCache* mc, TERMID_T id, int len);
  InvFPDocList(MemCache* mc, TERMID_T id, int len, DOCID_T docid, LOC_T location);
  /// constructor for a list not needing to get any memory
  InvFPDocList(TERMID_T id, int listlen, LOC_T* list, int fr, DOCID_T* ldocid, int len);
  ~InvFPDocList();

  bool addTerm(DOCID_T docid);
  bool addLocation(DOCID_T docid, LOC_T location);
  bool append(InvDocList* tail);

  virtual DocInfo* nextEntry() const;
  virtual void nextEntry(InvFPDocInfo* info) const;
  virtual COUNT_T termCTF() const;
  LOC_T *byteVec(COUNT_T &len);

protected:
  // Helper functions for iterator, subclasses should override
  /// create new element of this list type for the iterator
  virtual DocInfo* newElement() const { return new InvFPDocInfo(); }
  /// set element from position, returns pointer to the element
  virtual DocInfo* getElement(DocInfo* elem, POS_T position) const;
  /// copy values from one Element (of type InvFPDocInfo) to another
  virtual void assignElement(DocInfo* to, DocInfo* from) const { 
    *static_cast<InvFPDocInfo*>(to) = *static_cast<InvFPDocInfo*>(from); 
  }
  /// advance position
  virtual POS_T nextPosition(POS_T position) const;

  /// delta encode docids and positions from begin through end
  /// call before write
  virtual void deltaEncode();

  /// delta decode docids and positions from begin through end
  /// call after read
  virtual void deltaDecode();

private:
  mutable InvFPDocInfo entry;
};

#endif
