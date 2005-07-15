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

indri::index::DiskKeyfileVocabularyIterator::DiskKeyfileVocabularyIterator( int baseID, indri::file::BulkTreeReader& bulkTree, indri::thread::Mutex& lock, int fieldCount ) :
  _baseID(baseID),
  _bulkTree(bulkTree),
  _mutex(lock),
  _holdingLock(false),
  _fieldCount(fieldCount)
{
  _compressedData.write( disktermdata_size( _fieldCount ) * 2 );
  _decompressedData.write( disktermdata_size( _fieldCount ) );
  _bulkIterator = _bulkTree.iterator();
}

//
// DiskKeyfileVocabularyIterator destrcutor
//

indri::index::DiskKeyfileVocabularyIterator::~DiskKeyfileVocabularyIterator() {
  delete _bulkIterator;
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
}

//
// startIteration
//

void indri::index::DiskKeyfileVocabularyIterator::startIteration() {
  _acquire();
  int actual;

  _bulkIterator->startIteration();
  nextEntry();
}

//
// nextEntry
//

bool indri::index::DiskKeyfileVocabularyIterator::nextEntry() {
  _bulkIterator->nextEntry();
  if( _bulkIterator->finished() ) {
    _release();
    return false;
  }

  int actual;
  UINT32 key;

  _bulkIterator->get( key, _compressedData.front(), _compressedData.size(), actual );
  indri::utility::RVLDecompressStream stream( _compressedData.front(), actual );

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
  if( !_bulkIterator->finished() )
    return _diskTermData;

  return 0;
}

//
// finished
//

bool indri::index::DiskKeyfileVocabularyIterator::finished() {
  return _bulkIterator->finished();
}

