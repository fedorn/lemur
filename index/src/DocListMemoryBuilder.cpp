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
// DocListMemoryBuilder.cpp
//
// tds - 17 December 2003
//

#include "indri/DocListMemoryBuilder.hpp"
const int MIN_SIZE = 64;
const int GROW_TIMES = 12;
const int REQUIRED_ADD_DOCUMENT_SIZE = 15;

using namespace indri::index;

DocListMemoryBuilder::iterator DocListMemoryBuilder::getIterator() {
  return iterator( _lists );
}

size_t DocListMemoryBuilder::memorySize() const {
  size_t total = 0;

  // the lists follow the sequence MIN_SIZE, MIN_SIZE*2, MIN_SIZE*4, etc.
  // so, total size is (2^(lists+1)-1)*MIN_SIZE.
  int truncLists = std::min<int>( _lists.size(), GROW_TIMES );
  total = ((1 << (truncLists+1)) - 1) * MIN_SIZE;

  // each remaining list is max size
  int remainingLists = std::max<int>( (int)_lists.size() - GROW_TIMES, 0 );
  total += (MIN_SIZE << GROW_TIMES) * remainingLists;

  return total;
}

int DocListMemoryBuilder::documentFrequency() const {
  return _documentFrequency;
}

int DocListMemoryBuilder::termFrequency() const {
  return _termFrequency;
}

DocListMemoryBuilder::DocListMemoryBuilder() {
  _listBegin = 0;
  _listEnd = 0;
  _list = 0;
  _locationCountPointer = 0;

  clear();
}

void DocListMemoryBuilder::_copy( DocListMemoryBuilder& other ) {
  assert( other._listBegin == 0 || ( other._listBegin && other._listEnd && other._list ) );
  assert( other._locationCountPointer == 0 || ( other._list > other._locationCountPointer && other._listBegin < other._locationCountPointer ) );

  _lists = other._lists;
  _listBegin = other._listBegin;
  _listEnd = other._listEnd;
  _list = other._list;
  _locationCountPointer = other._locationCountPointer;

  _documentFrequency = other._documentFrequency;
  _termFrequency = other._termFrequency;

  _lastDocument = other._lastDocument;
  _lastLocation = other._lastLocation;
  _lastTermFrequency = other._lastTermFrequency;
}

const DocListMemoryBuilder& DocListMemoryBuilder::operator=( DocListMemoryBuilder& other ) {
  clear();
  _copy( other );

  // clear out the old one so the memory won't get deleted
  other._listBegin = 0;
  other._lists.clear();
  other.clear();
  return *this;
}

void DocListMemoryBuilder::_grow() {
  unsigned int iterations = std::min<unsigned int>( GROW_TIMES, _lists.size() );
  size_t newSize = (MIN_SIZE << iterations) - 8; // subtract 8 here to give the heap some room for accounting

  _terminateSegment();

  _list = new char[ newSize ];
  _listBegin = _list;
  _listEnd = _list + newSize;
}

void DocListMemoryBuilder::_createDocument( int docID ) {
  if( _list != _listBegin || _lists.size() )
    _terminateDocument();

  // store document ID in list
  _list = RVLCompress::compress_int( _list, docID - _lastDocument );

  // save one byte for location information
  _locationCountPointer = _list;
  _list++;

  _lastDocument = docID;
  _lastLocation = 0;
  _documentFrequency++;
}

void DocListMemoryBuilder::_terminateDocument() {
  assert( _locationCountPointer );
  int locations = _termFrequency - _lastTermFrequency;
  int locationsSize = RVLCompress::compressedSize( locations );

  if( locationsSize > 1 ) {
    // have to move everything around to make room, because we need more than
    // one byte to store this length.

    // first, we have to find the end pointer for this buffer, because it might
    // not be the buffer that we're adding to right now
    if( _listBegin <= _locationCountPointer &&
        _listEnd >= _locationCountPointer )
    {
      // yes, we're still in the current buffer, so _list is the one we want
      memmove( _locationCountPointer + locationsSize,
               _locationCountPointer + 1,
               _list - (_locationCountPointer + 1) );
      _list += locationsSize - 1;
    } else {
      // we're in some old buffer, so go find it
      assert( _lists.size() );
      bool moved = false;

      for( int i=_lists.size()-1; i>=0; i-- ) {
        if( _lists[i].first < _locationCountPointer &&
            _lists[i].second > _locationCountPointer )
        {
          memmove( _locationCountPointer + locationsSize,
                   _locationCountPointer + 1,
                   _lists[i].second - (_locationCountPointer + 1) );
          _lists[i].second += locationsSize - 1;
          moved = true;
          break;
        }
      }
      assert(moved);
    }
  }

  // we left one byte around for the location count for the common case
  RVLCompress::compress_int( _locationCountPointer, locations );
  _lastTermFrequency = _termFrequency;
  _locationCountPointer = 0;
}

void DocListMemoryBuilder::_terminateSegment() {
  // add old list to lists vector
  if( _listBegin )
    _lists.push_back( std::make_pair( _listBegin, _list ) );
  _listBegin = 0;
}

void DocListMemoryBuilder::_writeLocation( int location ) {
  _list = RVLCompress::compress_int( _list, location - _lastLocation );
  _lastLocation = location;
  _termFrequency++;
}

void DocListMemoryBuilder::addLocation( int docID, int location ) {
  if( _listEnd - _list < REQUIRED_ADD_DOCUMENT_SIZE )
    _grow();

  if( _lastDocument != docID )
    _createDocument( docID );

  _writeLocation( location );
}

void DocListMemoryBuilder::close() {
  if( _termFrequency != _lastTermFrequency ) {
    _terminateDocument();
  }
  _terminateSegment();
}

void DocListMemoryBuilder::clear() {
  delete[] _listBegin;
  _listBegin = 0;
  _listEnd = 0;
  _list = 0;

  _documentFrequency = 0;
  _termFrequency = 0;

  _lastLocation = 0;
  _lastDocument = 0;
  _lastTermFrequency = 0;
  _locationCountPointer = 0;

  for( size_t i=0; i<_lists.size(); i++ ) {
    delete _lists[i].first;
  }
  _lists.clear();
}

bool DocListMemoryBuilder::empty() {
  return _list == _listBegin && _lists.size() == 0;
}

