/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
*/


//
// IndriTermInfoList.cpp
//
// 13 August 2004 -- tds
// 17 August 2004 -- dmf separate implementation from header.
//


#include "indri/TermListBuilder.hpp"
#include "indri/IndriTermInfoList.hpp"
#include <indri/count_iterator>

namespace indri {
  namespace index {
    BagList::BagList( TermListBuilder* list ) : _list(list)
    {
      greedy_vector<TERMID_T> termIDs = _list->terms();
      count_iterator<TERMID_T> iter( termIDs.begin(), termIDs.end() );
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
      return _position < _termCounts.size();
    }

    TermInfo* BagList::nextEntry() const {
      getElement( &const_cast<BagList*>(this)->_info, POS_T(_position) );
      const_cast<BagList*>(this)->_position++;
      return &const_cast<BagList*>(this)->_info;
    }

    TermInfo* BagList::getElement( TermInfo* elem, POS_T position ) const {
      int p = int(position);
      elem->termID( _termCounts[p].first );
      elem->count( _termCounts[p].second );
      return elem;
    }

    POS_T BagList::beginPosition() const {
      return 0;
    }

    POS_T BagList::endPosition() const {
      return POS_T(_termCounts.size());
    }

    POS_T BagList::nextPosition( POS_T position ) const {
      return POS_T(int(position)+1);
    }

    PositionList::PositionList( TermListBuilder* list ) : _list(list) {}
    PositionList::~PositionList() { delete _list; }
      
    TermInfo* PositionList::newElement() {
      return new PositionInfo();
    }

    void PositionList::assignElement( TermInfo* to, TermInfo* from ) {
      *static_cast<PositionInfo*>(to) = *static_cast<PositionInfo*>(from);
    }

    TermInfo* PositionList::getElement( TermInfo* elem, POS_T position ) const {
      int p = int(position);
      static_cast<PositionInfo*>(elem)->position(p);
      elem->termID( _list->terms()[p] );
      elem->count(1);
      return elem;
    }

    POS_T PositionList::beginPosition() const {
      // list may start with stopwords.
      int p = 0;
      while (p < _list->terms().size() && _list->terms()[p] == 0) p++;
      return POS_T(p);
    }

    POS_T PositionList::endPosition() const {
      return POS_T(_list->terms().size());
    }

    POS_T PositionList::nextPosition( POS_T position ) const {
      // list may contain stopwords
      int p = int(position) + 1;
      while (_list->terms()[p] == 0 && p < _list->terms().size()) p++;
      return POS_T(p);
    }

    void PositionList::startIteration() const {
      // list may start with stopwords.
      int p = 0;
      while (p < _list->terms().size() && _list->terms()[p] == 0) p++;
      const_cast<PositionList*>(this)->_position = p;
    }

    bool PositionList::hasMore() const {
      // list may contain stopwords. 
      while (_position < _list->terms().size() &&
	     _list->terms()[_position] == 0) 
	const_cast<PositionList*>(this)->_position++;
      return _position < _list->terms().size();
    }

    TermInfo* PositionList::nextEntry() const {
      getElement( &const_cast<PositionList*>(this)->_info, POS_T(_position) );
      const_cast<PositionList*>(this)->_position++;
      return &const_cast<PositionList*>(this)->_info;
    }
  };
};



