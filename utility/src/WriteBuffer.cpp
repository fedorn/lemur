/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// WriteBuffer
//
// tds - 13 November 2003
//

#include "WriteBuffer.hpp"

size_t nearest_megabyte( size_t length ) {
  const int ONE_MEGABYTE = 1024*1024;
  return ( length + ONE_MEGABYTE - 1 ) & ~(ONE_MEGABYTE-1);
}

WriteBuffer::WriteBuffer( File& file, size_t bufferSize, bool exclusiveAccess ) : _file( file ) {
  _position = 0;
  _filePos = 0;
  _bufferSize = bufferSize;
  _buffer = (char*) malloc( bufferSize );
  _exclusiveAccess = exclusiveAccess;
}

WriteBuffer::~WriteBuffer() {
  free( _buffer );
}

char* WriteBuffer::write( size_t length ) {
  char* region = 0;

  // grow buffer if necessary
  if( (length + _position) <= _bufferSize ) {
    // buffer size is fine; don't need to do anything
  } else if( length < _bufferSize ) {
    // the buffer is large enough to hold this data, as
    // long as we flush the buffer first
    flush();
  } else {
    // the buffer is too small to hold this data, have to grow it
    flush();
    size_t newBufferSize = nearest_megabyte( length );
    char* newBuffer = (char*) malloc( newBufferSize );
    free( _buffer );
    _buffer = newBuffer;
    _bufferSize = newBufferSize;
  }

  region = _buffer + _position;
  _position += length;
  return region;
}

void WriteBuffer::unwrite( size_t length ) {
  assert( _position > length );
  _position -= length;
}

void WriteBuffer::write( const char* data, size_t length ) {
  if( (length + _position) < _bufferSize ) {
    memcpy( _buffer + _position, data, length );
    _position += length;
  } else {
    flush();
    _file.write(data, length);
    if (! _exclusiveAccess)
      _filePos = _file.tellp();
    else
      _filePos += length;
  }
}

void WriteBuffer::flush() {
  if( _position != 0 ) {
    _file.write( _buffer, _position );
    if (! _exclusiveAccess)
      _filePos = _file.tellp();
    else
      _filePos += _position;
    _position = 0;
  }
}

File::offset_type WriteBuffer::tellp() {
  if (! _exclusiveAccess)
    _filePos = _file.tellp();
  return _filePos + _position;
}
