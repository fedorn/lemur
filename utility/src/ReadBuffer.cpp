/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


//
// ReadBuffer
//
// tds - 13 November 2003
//

#include "ReadBuffer.hpp"
#include <string.h>
#include <iostream>

// This must be a power of two
#define READBUFFER_PAGE_SIZE      (4096)
#define READBUFFER_EXTENSION_SIZE (128*1024)

ReadBuffer::ReadBuffer( File& file, size_t bufferSize, bool exclusiveAccess ) : _file( file ) {
  _bufferPosition = 0;
  _filePosition = 0;
  // round up to the nearest multiple of pages
  _bufferSize = ( bufferSize + READBUFFER_PAGE_SIZE - 1 ) & ~( READBUFFER_PAGE_SIZE - 1 );
  _buffer = (char*) malloc( _bufferSize );
  _bufferDataLength = 0;
  _gValid = false;
  _exclusiveAccess = exclusiveAccess;
}

ReadBuffer::~ReadBuffer() {
  free( _buffer );
}

void ReadBuffer::read( char* data, size_t length ) {
  // read at least as much as we have left in the buffer, first
  size_t bufferCopyLength = lemur_compat::min(_bufferDataLength - _bufferPosition, length );
  memcpy( data, _buffer + _bufferPosition, bufferCopyLength );
  _bufferPosition += bufferCopyLength;

  // did we read everything we needed to read?
  if( bufferCopyLength != length ) {
    // reset the buffer to zero
    _filePosition += _bufferDataLength;
    _bufferPosition = 0;
    _bufferDataLength = 0;

    // is the amount that we have to read very big?
    // if so, we should probably do a buffered read
    // if not, we'll do a direct read

    size_t remainingLength = length - bufferCopyLength;

    if( remainingLength > _bufferSize/2 ) {
      _file.read( data+bufferCopyLength, remainingLength );
      _filePosition += _file.gcount();
    } else {
      // the file stream may not be in the correct place for reading
      // seek in the file if necessary
      if( !_gValid ) {
        _file.seekg( _filePosition, std::ios::beg );
        _gValid = _exclusiveAccess;
      }

      // fill buffer
      _file.read( _buffer, _bufferSize );
      _bufferDataLength = static_cast<size_t>(_file.gcount());

      size_t finalCopyLength = lemur_compat::min( remainingLength, _bufferDataLength );
      memcpy( data+bufferCopyLength, _buffer, finalCopyLength );
      _bufferPosition = finalCopyLength;
    }
  }
}

const char* ReadBuffer::peek( size_t length ) {
  if( (_bufferPosition + length) <= _bufferDataLength ) {
    // the data is already in the buffer, so return the pointer
    return _buffer + _bufferPosition;
  } else {
    // pick a page aligned address to move to keep user/os copied page aligned
    size_t pagePosition = _bufferPosition & ~(READBUFFER_PAGE_SIZE-1);
    // remainingDataLength is the amount of data left in the buffer after we remove the old stuff
    size_t remainingDataLength = _bufferDataLength - pagePosition;

    if( length <= (_bufferSize - remainingDataLength) ) {
      // buffer is big enough, just move stuff around to make room for new data
      ::memmove( _buffer, _buffer + pagePosition, remainingDataLength );
    } else {
      // buffer is not big enough to hold the new data, so make a bigger buffer
      // and copy any remaining old data into it
      size_t requiredLength = remainingDataLength + length;
      size_t largerSize = requiredLength + READBUFFER_EXTENSION_SIZE-(requiredLength&(READBUFFER_EXTENSION_SIZE-1));
      char* largerBuffer = (char*) malloc( largerSize );

      ::memcpy( largerBuffer, _buffer + pagePosition, remainingDataLength );

      _bufferSize = largerSize;
      free( _buffer );
      _buffer = largerBuffer;
    }

    // update buffer pointers to reflect the data shift
    _bufferPosition -= pagePosition;
    _filePosition += pagePosition;
    _bufferDataLength -= pagePosition;

    // the file stream may not be in the correct place for reading
    // seek in the file if necessary
    if( !_gValid ) {
      _file.seekg( _filePosition + _bufferDataLength, std::ios::beg );
      _gValid = _exclusiveAccess;
    }

    // read in some new data to fill the void
    _file.read( _buffer + _bufferDataLength, _bufferSize - _bufferDataLength );
    _bufferDataLength += static_cast<size_t>(_file.gcount());
  }

  // now, double check to make sure the data actually arrived
  // before returning a pointer to it
  if( (_bufferPosition + length) > _bufferDataLength ) {
    return 0;
  } else {
    return _buffer + _bufferPosition;
  }
}

const char* ReadBuffer::read( size_t length ) {
  const char* ptr = peek(length);
  _bufferPosition += length;

  return ptr;
}

File::offset_type ReadBuffer::tellg() {
  return _filePosition + _bufferPosition;
}

void ReadBuffer::seekg( File::offset_type position, std::fstream::seekdir direction ) {
  File::offset_type absolutePosition;

  switch( direction ) {
    default:
    case std::fstream::beg:
      absolutePosition = position;
      break;

    case std::fstream::cur:
      absolutePosition = _filePosition + position;
      break;

    case std::fstream::end:
      absolutePosition = _file.size() + position;
      break;
  }

  assert( absolutePosition <= _file.size() );

  // bugbug: this screws up the page alignment stuff somewhat
  if( absolutePosition >= _filePosition && absolutePosition <= (_bufferDataLength + _filePosition) ) {
    _bufferPosition = size_t(absolutePosition - _filePosition);
  } else {
    _bufferDataLength = 0;
    _bufferPosition = 0;
    _filePosition = absolutePosition;
    _file.seekg( _filePosition, std::ios::beg );
    _gValid = _exclusiveAccess;
  }
}

int ReadBuffer::rdstate() {
  if( _bufferPosition < _bufferDataLength )
    return 0;
  else
    return _file.rdstate();
}

void ReadBuffer::invalidateg() {
  _gValid = false;
}
