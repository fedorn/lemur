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
#include "minmax.hpp"
#include <string>

//#define READBUFFER_PAGE_SIZE (4096)
#define READBUFFER_PAGE_SIZE (8192)

ReadBuffer::ReadBuffer( File& file, int bufferSize ) : _file( file ) {
  _bufferPosition = 0;
  _filePosition = 0;
  _bufferSize = bufferSize;
  _buffer = new char[bufferSize];
  _bufferDataLength = 0;
  _gValid = false;
}

ReadBuffer::~ReadBuffer() {
  delete[]( _buffer);
}

void ReadBuffer::read( char* data, int length ) {
  // read at least as much as we have left in the buffer, first
//  int bufferCopyLength = std::min(_bufferDataLength - _bufferPosition, length );
	int bufferCopyLength = _MINIM(_bufferDataLength - _bufferPosition, length );
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

    int remainingLength = length - bufferCopyLength;

    if( remainingLength > _bufferSize/2 ) {
      _file.read( data+bufferCopyLength, remainingLength );
      _filePosition += _file.gcount();
    } else {
      // the file stream may not be in the correct place for reading
      // seek in the file if necessary
      if( !_gValid ) {
        _file.seekg( _filePosition, std::ios::beg );
        _gValid = true;
      }

      // fill buffer
      _file.read( _buffer, _bufferSize );
      _bufferDataLength = int(_file.gcount());

      int finalCopyLength = _MINIM( remainingLength, _bufferDataLength );
      memcpy( data+bufferCopyLength, _buffer, finalCopyLength );
      _bufferPosition = finalCopyLength;
    }
  }
}

const char* ReadBuffer::peek( int length ) {
  if( (_bufferPosition + length) <= _bufferDataLength ) {
    // the data is already in the buffer, so return the pointer
    return _buffer + _bufferPosition;
  } else {
    // pick a page aligned address to move to keep user/os copied page aligned
    int pagePosition = _bufferPosition & ~(READBUFFER_PAGE_SIZE-1);
    // remainingDataLength is the amount of data left in the buffer after we remove the old stuff
    int remainingDataLength = _bufferDataLength - pagePosition;

    if( length <= (_bufferSize - remainingDataLength) ) {
      // buffer is big enough, just move stuff around to make room for new data
      ::memmove( _buffer, _buffer + pagePosition, remainingDataLength );
    } else {
      // buffer is not big enough to hold the new data, so make a bigger buffer
      // and copy any remaining old data into it
      int requiredLength = remainingDataLength + length;
      int largerSize = requiredLength + (128*1024)-(requiredLength&(128*1024-1));
      char* largerBuffer = new char[largerSize];

      ::memcpy( largerBuffer, _buffer + pagePosition, remainingDataLength );

      _bufferSize = largerSize;
      delete[](_buffer);
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
      _gValid = true;
    }

    // read in some new data to fill the void
    _file.read( _buffer + _bufferDataLength, _bufferSize - _bufferDataLength );
    _bufferDataLength += int(_file.gcount());
  }

  // now, double check to make sure the data actually arrived
  // before returning a pointer to it
  if( (_bufferPosition + length) > _bufferDataLength ) {
    return 0;
  } else {
    return _buffer + _bufferPosition;
  }
}

const char* ReadBuffer::read( int length ) {
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

  //  assert( absolutePosition <= _file.size() );

  // bugbug: this screws up the page alignment stuff somewhat
  if( absolutePosition >= _filePosition && absolutePosition <= (_bufferDataLength + _filePosition) ) {
    _bufferPosition = int(absolutePosition - _filePosition);
  } else {
    _bufferDataLength = 0;
    _bufferPosition = 0;
    _filePosition = absolutePosition;
    _file.seekg( _filePosition, std::ios::beg );
    _gValid = true;
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
