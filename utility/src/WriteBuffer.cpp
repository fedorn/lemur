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
// WriteBuffer
//
// tds - 13 November 2003
//

#include "WriteBuffer.hpp"

WriteBuffer::WriteBuffer( File& file, int bufferSize ) : _file( file ) {
  _position = 0;
  _filePos = 0;
  _bufferSize = bufferSize;
  _buffer = new char[bufferSize];
}

WriteBuffer::~WriteBuffer() {
  delete[](_buffer);
}

void WriteBuffer::write( const char* data, int length ) {
  if( (length + _position) < _bufferSize ) {
    memcpy( _buffer + _position, data, length );
    _position += length;
  } else {
    flush();
    _file.write(data, length);
    _filePos += length;
  }
}

void WriteBuffer::flush() {
  if( _position != 0 ) {
    _file.write( _buffer, _position );
    _filePos += _position;
    _position = 0;
  }
}

File::offset_type WriteBuffer::tellp() const {
  return _filePos + _position;
}
