/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// RVLCompressStream
//
// 9 February 2004 -- tds
//

#ifndef INDRI_RVLCOMPRESSSTREAM_HPP
#define INDRI_RVLCOMPRESSSTREAM_HPP

/*! Provide RVL compression on a stream.
 */
class RVLCompressStream {
private:
  char* _buffer;
  size_t _bufferSize;
  char* _current;

public:
  /// Initialize
  /// @param buffer the buffer to use for compressing
  /// @param size the size of buffer
  RVLCompressStream( char* buffer, size_t size ) {
    _buffer = buffer;
    _bufferSize = size;
    _current = _buffer;
  }

  /// @return the buffer
  const char* data() const {
    return _buffer;
  }
  /// @return the size of the data in the buffer
  size_t dataSize() const {
    return _current - _buffer;
  }

  /// Compress an int into the buffer
  /// @param value the value to compress
  RVLCompressStream& operator<< ( int value ) {
    _current = RVLCompress::compress_int( _current, value );
    assert( _bufferSize >= size_t(_current - _buffer) );
    return *this;
  }

  /// Compress an unsigned int into the buffer
  /// @param value the value to compress
  RVLCompressStream& operator<< ( unsigned int value ) {
    _current = RVLCompress::compress_int( _current, value );
    assert( _bufferSize >= size_t(_current - _buffer) );
    return *this;
  }

  /// Compress an INT64 into the buffer
  /// @param value the value to compress
  RVLCompressStream& operator<< ( INT64 value ) {
    _current = RVLCompress::compress_longlong( _current, value );
    assert( _bufferSize >= size_t(_current - _buffer) );
    return *this;
  }

  /// Compress a float into the buffer
  /// @param value the value to compress
  RVLCompressStream& operator << ( float value ) {
    // can't compress a float, unfortunately
    memcpy( _current, &value, sizeof value );
    _current += sizeof value;
    return *this;
  }
};

#endif // INDRI_RVLCOMPRESSSTREAM_HPP

