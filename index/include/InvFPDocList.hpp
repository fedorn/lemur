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
namespace lemur 
{
  namespace index 
  {
    
    class InvFPDocList: public InvDocList {
    public:
      InvFPDocList();
      /// copy from byteVector
      InvFPDocList(lemur::api::LOC_T *vec);

      /// constructor for this list using malloc for its own memory
      /// usage of InvFPDocList without MemCache has not been tested
      InvFPDocList(lemur::api::TERMID_T id, int len);
      /// constructors for this list getting memory from a MemCache
      InvFPDocList(lemur::utility::MemCache* mc, lemur::api::TERMID_T id, int len);
      InvFPDocList(lemur::utility::MemCache* mc, lemur::api::TERMID_T id, int len, lemur::api::DOCID_T docid, lemur::api::LOC_T location);
      /// constructor for a list not needing to get any memory
      InvFPDocList(lemur::api::TERMID_T id, int listlen, lemur::api::LOC_T* list, int fr, lemur::api::DOCID_T* ldocid, int len);
      ~InvFPDocList();

      bool addTerm(lemur::api::DOCID_T docid);
      bool addLocation(lemur::api::DOCID_T docid, lemur::api::LOC_T location);
      bool append(InvDocList* tail);

      virtual lemur::api::DocInfo* nextEntry() const;
      virtual void nextEntry(InvFPDocInfo* info) const;
      virtual lemur::api::COUNT_T termCTF() const;
      lemur::api::LOC_T *byteVec(lemur::api::COUNT_T &len);

    protected:
      // Helper functions for iterator, subclasses should override
      /// create new element of this list type for the iterator
      virtual lemur::api::DocInfo* newElement() const { return new InvFPDocInfo(); }
      /// set element from position, returns pointer to the element
      virtual lemur::api::DocInfo* getElement(lemur::api::DocInfo* elem, lemur::api::POS_T position) const;
      /// copy values from one Element (of type InvFPDocInfo) to another
      virtual void assignElement(lemur::api::DocInfo* to, lemur::api::DocInfo* from) const { 
        *static_cast<InvFPDocInfo*>(to) = *static_cast<InvFPDocInfo*>(from); 
      }
      /// advance position
      virtual lemur::api::POS_T nextPosition(lemur::api::POS_T position) const;

      /// delta encode docids and positions from begin through end
      /// call before write
      virtual void deltaEncode();

      /// delta decode docids and positions from begin through end
      /// call after read
      virtual void deltaDecode();

    private:
      mutable InvFPDocInfo entry;
    };
  }
}

#endif
