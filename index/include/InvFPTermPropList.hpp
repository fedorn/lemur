/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _INVPROPTERMLIST_HPP
#define _INVPROPTERMLIST_HPP

#include "InvFPTermList.hpp"

namespace lemur 
{
  namespace index 
  {
    
    /* Some additional methods to make use of lists with properties in them a bit
     * easier to deal with.  We assume that you have a term, and all properties
     * pertaining to a certain term indexed with the same position as what
     * PropIndexTH does. Term comes first, all properties for that term indexed
     * afters.
     * This is to be used with sequence (not bad of words) InvFPTermList as that
     * one would get from calling InvFPIndex::TermInfoListSeq(). Use with bag of
     * word lists will return error values.
     *
     */
    class InvFPTermPropList: public InvFPTermList {
    public:

      /// Skip all tags and give me the next term
      /// Basicaly skips to first item with different position
      /// Returns NULL if no more terms.
      lemur::api::TermInfo* nextTerm() const;
      lemur::api::TermInfo* nextTerm(InvFPTermPropList::iterator &it) const;

      /// returns true if there is another term
      /// can use to know that nextTerm will not return NULL
      /// check is slower than hasMore(), depending on number of properties
      bool hasMoreTerm() const;
      bool hasMoreTerm(InvFPTermPropList::iterator &it) const;

      /// Advance the pointer to the next entry with the given position
      /// goes to next available term past pos if no actually pos is found
      /// (can happen because of stopwords)
      /// goes to end if pos is beyond end of list
      void skipTo(int pos) const;
      void skipTo(InvFPTermPropList::iterator &it, int pos) const;

      /// Peek ahead and tell me what the position of the next entry is
      /// Does not change position of iterator
      /// returns -1 on error or end of list
      int nextEntryPos() const;
      int nextEntryPos(InvFPTermPropList::iterator &it) const;

      /// Peak ahead and tell me what the position of the next term is
      /// (skips all entries with same position and gives the next different position)
      /// Does not change position of iterator
      /// returns -1 on error or end of list
      int nextTermPos() const;
      int nextTermPos(InvFPTermPropList::iterator &it) const;

    };
  }
}

#endif
