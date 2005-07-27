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

#ifndef _INVTERMLIST_HPP
#define _INVTERMLIST_HPP

#include "common_headers.hpp"
#include "InvFPTerm.hpp"
#include "InvFPTypes.hpp"
namespace lemur 
{
  namespace index 
  {
    

    /*! 

    This is a class that represents the term entries in doc index, to be used with InvFPIndex
    i.e., the ID and frequency of all terms in a document.

    */

    class InvTermList : public lemur::api::TermInfoList {
    public:
      InvTermList();
      ~InvTermList();

      /// prepare iteration
      void startIteration() const;

      /// has more entries
      bool hasMore() const;

      /// Get a pointer to the next entry (pointer to a local static memory)
      lemur::api::TermInfo *nextEntry() const;

      /// Get the length of this document
      lemur::api::COUNT_T docLength() const { return length; }

      /// Get the number of terms in this document
      lemur::api::COUNT_T termCount() const{ return listlen/2; }

      /// Get the docid this list is for
      lemur::api::DOCID_T docID() const{ return uid; }

      /// Read in a TermInfoList object from a file
      /// Assumes the following format: DOCID DOCLENGTH TID TERM_COUNT ..
      bool binRead(ifstream& infile);

    protected:
      // Helper functions for iterator, subclasses should override
      /// set element from position, returns pointer to the element
      virtual lemur::api::TermInfo* getElement(lemur::api::TermInfo* elem, lemur::api::POS_T position) const;
      /// position at beginning of list
      virtual lemur::api::POS_T beginPosition() const { return (lemur::api::POS_T) 0; }
      /// position at end of list
      virtual lemur::api::POS_T endPosition() const { return (lemur::api::POS_T) (end - list); }
      /// advance position
      virtual lemur::api::POS_T nextPosition(lemur::api::POS_T position) const;

    private:

      lemur::api::DOCID_T uid; // this doc's id
      int length;  // length of this document (terms + stopwords)
      // LOC_T==TERMID_T==COUNT_T==DOCID_T
      lemur::api::LOC_T* list; // beginning of list of terms and counts
      lemur::api::LOC_T* end; // end of list
      int listlen; // number of terms in list
      mutable lemur::api::LOC_T* iter;   // index for iterator
      mutable lemur::api::TermInfo entry;
    };
  }
}
#endif
