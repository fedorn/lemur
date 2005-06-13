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
// DeletedDocumentList
//
// 3 February 2005 -- tds
//

#include "indri/DeletedDocumentList.hpp"
#include "indri/File.hpp"
#include "Exception.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/Path.hpp"

//
// DeletedDocumentList constructor
//

indri::index::DeletedDocumentList::DeletedDocumentList() :
  _readLock( _lock ),
  _writeLock( _lock )
{
}

//
// read_transaction constructor
//

indri::index::DeletedDocumentList::read_transaction::read_transaction( DeletedDocumentList& list ) :
  _lock(list._lock),
  _bitmap(list._bitmap)
{
  _lock.lockRead();
}

//
// ~read_transaction constructor
//

indri::index::DeletedDocumentList::read_transaction::~read_transaction() {
  _lock.unlockRead();
}

//
// grow
//

void indri::index::DeletedDocumentList::_grow( int documentID ) {
  // just set an appropriate number of bytes to zero
  int growBytes = (documentID/8)+1 - _bitmap.position();
  memset( _bitmap.write( growBytes ), 0, growBytes );

  assert( _bitmap.position() > (documentID/8) );
}

//
// nextDocument
//

int indri::index::DeletedDocumentList::read_transaction::nextCandidateDocument( int documentID ) {
  _lock.yieldRead();

  while( documentID < _bitmap.position()*8 ) {
    char bitmapByte = _bitmap.front()[documentID/8];
    bool marked = (bitmapByte & 1<<(documentID%8)) != 0;

    if( !marked )
      break;

    documentID++;
  }

  return documentID;
}

//
// isDeleted
//

bool indri::index::DeletedDocumentList::read_transaction::isDeleted( int documentID ) {
  if( _bitmap.position() < (documentID/8)+1 )
    return false;

  char bitmapByte = _bitmap.front()[documentID/8];
  bool marked = (bitmapByte & 1<<(documentID%8)) != 0;

  return marked;
}

//
// markDeleted
//

void indri::index::DeletedDocumentList::markDeleted( int documentID ) {
  indri::thread::ScopedLock l( _writeLock );

  if( _bitmap.position() < (documentID/8)+1 ) {
    _grow( documentID );
  }

  _bitmap.front()[documentID/8] |= 1<<(documentID%8);
}

//
// isDeleted
//

bool indri::index::DeletedDocumentList::isDeleted( int documentID ) {
  indri::thread::ScopedLock l( _readLock );
  if( _bitmap.position() < (documentID/8)+1 )
    return false;

  char bitmapByte = _bitmap.front()[documentID/8];
  bool marked = (bitmapByte & 1<<(documentID%8)) != 0;

  return marked;
}

//
// getReadTransaction
//

indri::index::DeletedDocumentList::read_transaction* indri::index::DeletedDocumentList::getReadTransaction() {
  return new read_transaction( *this );
}

//
// read
//

void indri::index::DeletedDocumentList::read( const std::string& filename ) {
  indri::file::File file;

  if( !file.openRead( filename ) )
    LEMUR_THROW( LEMUR_IO_ERROR, "Unable to open file: " + filename );

  UINT64 fileSize = file.size();
  _bitmap.clear();
  file.read( _bitmap.write( fileSize ), 0, fileSize );
  file.close();
}

//
// write
//

void indri::index::DeletedDocumentList::write( const std::string& filename ) {
  indri::file::File file;

  if( indri::file::Path::exists( filename ) )
    indri::file::Path::remove( filename );
  if( !file.create( filename ) )
    LEMUR_THROW( LEMUR_IO_ERROR, "Unable to create file: "  + filename );

  file.write( _bitmap.front(), 0, _bitmap.position() );
  file.close();
}
