/*==========================================================================
 * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

//
// DiskFrequentVocabularyIterator
//
// 19 January 2005 -- tds
//

#include "indri/DiskFrequentVocabularyIterator.hpp"
#include "indri/DiskTermData.hpp"

//
// DiskFrequentVocabularyIterator constructor
//

indri::index::DiskFrequentVocabularyIterator::DiskFrequentVocabularyIterator( indri::file::File& frequentTermsData, int fieldCount ) :
  _file(frequentTermsData),
  _fieldCount(fieldCount),
  _stream(0, 0)
{
}

//
// startIteration
//

void indri::index::DiskFrequentVocabularyIterator::startIteration() {
  if( _buffer.size() == 0 ) {
    UINT64 length = _file.size();
    _file.read( _buffer.write( length ), 0, length );
  }

  _finished = false;
  _stream.setBuffer( _buffer.front(), _buffer.position() );
  nextEntry();
}

//
// nextEntry
//

bool indri::index::DiskFrequentVocabularyIterator::nextEntry() {
  if( !_stream.done() ) {
    _data = ::disktermdata_decompress( _stream, _dataBuffer, _fieldCount, indri::index::DiskTermData::WithOffsets |
                                                                          indri::index::DiskTermData::WithTermID | 
                                                                          indri::index::DiskTermData::WithString );
    return true;
  } else {
    _finished = true;
    return false;
  }
}

//
// finished
//

bool indri::index::DiskFrequentVocabularyIterator::finished() {
  return _finished;
}

//
// currentEntry
//

indri::index::DiskTermData* indri::index::DiskFrequentVocabularyIterator::currentEntry() {
  if( !_finished )
    return _data;

  return 0;
}
