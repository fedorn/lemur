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
// DiskKeyfileVocabularyIterator
//
// 20 January 2005 -- tds
//

#include "indri/DiskKeyfileVocabularyIterator.hpp"
#include "indri/Mutex.hpp"

//
// DiskKeyfileVocabularyIterator constructor
//

indri::index::DiskKeyfileVocabularyIterator::DiskKeyfileVocabularyIterator( int baseID, indri::file::BulkTreeReader& keyfile, indri::thread::Mutex& lock, int fieldCount ) :
  _baseID(baseID),
  _keyfile(keyfile),
  _mutex(lock),
  _holdingLock(false),
  _finished(true),
  _fieldCount(fieldCount)
{
  _compressedData.write( disktermdata_size( _fieldCount ) * 2 );
  _decompressedData.write( disktermdata_size( _fieldCount ) );
}

//
// _acquire
//

void indri::index::DiskKeyfileVocabularyIterator::_acquire() {
  if( !_holdingLock ) {
    _mutex.lock();
    _holdingLock = true;
  }
}

//
// _release
//

void indri::index::DiskKeyfileVocabularyIterator::_release() {
  if( _holdingLock ) {
    _mutex.unlock();
    _holdingLock = false;
  }

  _finished = false;
}

//
// startIteration
//

void indri::index::DiskKeyfileVocabularyIterator::startIteration() {
  _acquire();
  assert( 0 && "doesn't work" );

  int actual;
  // fix me _finished = !_keyfile.get( 0, _compressedData.front(), actual, _compressedData.size() );
  indri::utility::RVLDecompressStream stream( _compressedData.front(), actual );

  if( _finished )
    return;

  _diskTermData = ::disktermdata_decompress( stream,
                                            _decompressedData.front(),
                                            _fieldCount,
                                            DiskTermData::WithOffsets |
                                            DiskTermData::WithString );
  _diskTermData->termID = _baseID + 0;
}

//
// nextEntry
//

bool indri::index::DiskKeyfileVocabularyIterator::nextEntry() {
  if( _finished )
    return false;
  assert( 0 && "doesn't work" );


  int key;
  int actual;
  // fix me _finished = !_keyfile.next( key, _compressedData.front(),  actual );
  indri::utility::RVLDecompressStream stream( _data, actual );

  if( _finished ) {
    _release();
    return false;
  }

  _diskTermData = ::disktermdata_decompress( stream,
                                            _decompressedData.front(),
                                            _fieldCount,
                                            DiskTermData::WithOffsets |
                                            DiskTermData::WithString );
  _diskTermData->termID = _baseID + key;

  return true;
}

//
// currentEntry
//

indri::index::DiskTermData* indri::index::DiskKeyfileVocabularyIterator::currentEntry() {
  if( !_finished )
    return _diskTermData;

  return 0;
}

//
// finished
//

bool indri::index::DiskKeyfileVocabularyIterator::finished() {
  return _finished;
}


