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
// DocListFrequencyIterator
//
// 24 August 2004 -- tds 
//

#include "indri/DocListFrequencyIterator.hpp"


indri::index::DocListFrequencyIterator::DocListFrequencyIterator( File& segment,
                                                                  int termID, 
                                                                  File::offset_type seekLocation,
                                                                  INT64 estimatedDataLength )
  :
  _finished(false),
  _termID(termID),
  _seekLocation(seekLocation),
  _buffer(new ReadBuffer(segment, lemur_compat::min<UINT64>(estimatedDataLength, 1024*1024), false))
{
  _iterator = _entries.begin();
  _end = _entries.end();
}

indri::index::DocListFrequencyIterator::~DocListFrequencyIterator() {
  delete _buffer;
}

void indri::index::DocListFrequencyIterator::_fetchNextBlock( int documentID ) {
  const char* block = _buffer->read( indri::index::DocListDiskBlockReader::blockSize() ); 

  if( block ) {
    _reader.setBlock( block );
    if( _reader.lastTerm() != _termID || _reader.lastDocument() >= documentID ) {
      // document might be here, so read these entries     
      _reader.readBlock( _termID, _entries );
      _finished = (_termID < _reader.lastTerm());
    }
  } else {
    _finished = true;
  }
}

void indri::index::DocListFrequencyIterator::_fillVector( int documentID ) {
  if( !_finished ) {
    // first, remove everything up to where the iterator is
    _entries.erase( _entries.begin(), _iterator );
    
    // next, check to see if the remaining entries are less than documentID
    // if so, remove them too
    for( _iterator = _entries.begin(); _iterator != _entries.end(); _iterator++ ) {
      if( _iterator->document >= documentID )
        break;
    }
    _entries.erase( _entries.begin(), _iterator );
    size_t lastSize = _entries.size();

    // get more than one additional entry; that way we at least know that
    // the first additional entry is good.  We'll also know that if we run
    // out of entries--when the last term of the block isn't the term we're looking for.
    do {
      _fetchNextBlock( documentID );
    } while( (lastSize+1) >= _entries.size() && !_finished );

    if( _finished ) {
      _end = _entries.end();
    } else {
      _end = _entries.end() - 1;
    }

    // reset the iterator, as the vector may have grown
    _iterator = _entries.begin();
  }
}

bool indri::index::DocListFrequencyIterator::hasMore() {
  return _iterator != _end;
}

void indri::index::DocListFrequencyIterator::startIteration() {
  _buffer->seekg(_seekLocation * indri::index::DocListDiskBlockReader::blockSize(), std::ios::beg);
  _fillVector(0);
}

void indri::index::DocListFrequencyIterator::nextEntry( int documentID ) {
  do {
    for( ; _iterator < _end; _iterator++ ) {
      if( documentID <= _iterator->document )
        return;
    }

    if( _iterator == _end && !_finished ) {
      _fillVector( documentID );
    }
  } while(_iterator != _end);
}

void indri::index::DocListFrequencyIterator::nextEntry() {
  if( _iterator == _end )
    return;

  _iterator++;

  if( _iterator == _end && !_finished ) {
    _fillVector( 0 );
  }
}

const DocumentCount* indri::index::DocListFrequencyIterator::currentEntry() {
  if( _iterator == _end )
    return 0;
  else
    return _iterator;
}
