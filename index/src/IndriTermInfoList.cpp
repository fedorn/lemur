/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// IndriTermInfoList.cpp
//
// 13 August 2004 -- tds
// 17 August 2004 -- dmf separate implementation from header.
//


#include "indri/TermList.hpp"
#include "indri/IndriTermInfoList.hpp"
#include <indri/count_iterator>

namespace indri {
  namespace index {
    BagList::BagList( const TermList* list ) : _list(list)
    {
      indri::utility::greedy_vector<lemur::api::TERMID_T> termIDs = _list->terms();
      indri::utility::count_iterator<lemur::api::TERMID_T> iter( termIDs.begin(), termIDs.end() );
      // indri indexes stopwords as [OOV], so skip those
      for( ; iter != termIDs.end(); ++iter ) {
        if ((*iter).object != 0)
          _termCounts.push_back( std::make_pair( (*iter).object, 
                                                 (*iter).count ) );
      }
    }
      
    BagList::~BagList() {
      delete _list;
    }

    void BagList::startIteration() const {
      const_cast<BagList*>(this)->_position = 0;
    }

    bool BagList::hasMore() const {
      return _position < (int)_termCounts.size();
    }

    lemur::api::TermInfo* BagList::nextEntry() const {
      getElement( &const_cast<BagList*>(this)->_info, lemur::api::POS_T(_position) );
      const_cast<BagList*>(this)->_position++;
      return &const_cast<BagList*>(this)->_info;
    }

    lemur::api::TermInfo* BagList::getElement( lemur::api::TermInfo* elem, 
                                               lemur::api::POS_T position ) const {
      int p = int(position);
      elem->termID( _termCounts[p].first );
      elem->count( _termCounts[p].second );
      return elem;
    }

    lemur::api::POS_T BagList::beginPosition() const {
      return 0;
    }

    lemur::api::POS_T BagList::endPosition() const {
      return lemur::api::POS_T(_termCounts.size());
    }

    lemur::api::POS_T BagList::nextPosition( lemur::api::POS_T position ) const {
      return lemur::api::POS_T(int(position)+1);
    }

    /// Gets the number of entries in this list
    int BagList::size() {
      return (int)_termCounts.size();
    }

    /// indexed access to the list. Also sets the iterator to the next item after
    /// or to the end if invalid.
    lemur::api::TermInfo* BagList::operator[](int index) const {
      if ((index < 0) || (index >= _termCounts.size())) {
        // invalid index - set the iterator to the end...
        const_cast<BagList*>(this)->_position=(int)_termCounts.size();
        return NULL;
      }

      // we're good - return the item at the index
      const_cast<BagList*>(this)->_position=index;
      return nextEntry();
    }

    PositionList::PositionList( const TermList* list ) : _list(list) {}
    PositionList::~PositionList() { delete _list; }
      
    lemur::api::TermInfo* PositionList::newElement() {
      return new PositionInfo();
    }

    void PositionList::assignElement( lemur::api::TermInfo* to, 
                                      lemur::api::TermInfo* from ) {
      *static_cast<PositionInfo*>(to) = *static_cast<PositionInfo*>(from);
    }

    lemur::api::TermInfo* PositionList::getElement( lemur::api::TermInfo* elem, lemur::api::POS_T position ) const {
      int p = int(position);
      static_cast<PositionInfo*>(elem)->position(p);
      elem->termID( _list->terms()[p] );
      elem->count(1);
      return elem;
    }

    lemur::api::POS_T PositionList::beginPosition() const {
      // list may start with stopwords.
      size_t p = 0;
      while (p < _list->terms().size() && _list->terms()[p] == 0) p++;
      return lemur::api::POS_T(p);
    }

    lemur::api::POS_T PositionList::endPosition() const {
      return lemur::api::POS_T(_list->terms().size());
    }

    lemur::api::POS_T PositionList::nextPosition( lemur::api::POS_T position ) const {
      // list may contain stopwords
      int p = int(position) + 1;
      while (_list->terms()[p] == 0 && p < (int)_list->terms().size()) p++;
      return lemur::api::POS_T(p);
    }

    void PositionList::startIteration() const {
      // list may start with stopwords.
      size_t p = 0;
      while (p < _list->terms().size() && _list->terms()[p] == 0) p++;
      const_cast<PositionList*>(this)->_position = (int)p;
    }

    bool PositionList::hasMore() const {
      // list may contain stopwords. 
      while (_position < (int)_list->terms().size() &&
             _list->terms()[_position] == 0) {
        const_cast<PositionList*>(this)->_position++;
      }
      return _position < (int)_list->terms().size();
    }

    lemur::api::TermInfo* PositionList::nextEntry() const {
      getElement( &const_cast<PositionList*>(this)->_info, lemur::api::POS_T(_position) );
      const_cast<PositionList*>(this)->_position++;
      return &const_cast<PositionList*>(this)->_info;
    }

    // Gets the number of entries in this list
    int PositionList::size() {
      return (int)_list->terms().size();
    }

    // indexed access to the list. Also sets the iterator to the next item after
    // or to the end if invalid.
    lemur::api::TermInfo* PositionList::operator[](int index) const {
      if ((index < 0) || (index >= (int)_list->terms().size())) {
        // invalid - set the iterator return null.
        const_cast<PositionList*>(this)->_position=(int)_list->terms().size();
        return NULL;
      }

      // else - we're good - set the iterator and return next
      const_cast<PositionList*>(this)->_position=index;
      return nextEntry();
    }
  }
}



