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
//
//
//
//

#ifndef INDRI_RVLDECOMPRESSSTREAM_HPP
#define INDRI_RVLDECOMPRESSSTREAM_HPP

/*! Provide RVL decompression on a stream.
 */
class RVLDecompressStream {
private:
  const char* _buffer;
  int _bufferSize;
  const char* _current;

public:
  /// Initialize
  /// @param buffer the buffer to use for decompressing
  /// @param size the size of buffer
  RVLDecompressStream( const char* buffer, int size ) {
    _buffer = buffer;
    _bufferSize = size;
    _current = buffer;
  }

  /// Decompress an INT64 from the buffer into value
  /// @param value reference to the container for the value.
  RVLDecompressStream& operator>> ( INT64& value ) {
    _current = RVLCompress::decompress_longlong( _current, value );
    assert( _current - _buffer <= _bufferSize );
    return *this;
  }

  /// Decompress an UINT64 from the buffer into value
  /// @param value reference to the container for the value.
  RVLDecompressStream& operator>> ( UINT64& value ) {
    INT64 other;
    _current = RVLCompress::decompress_longlong( _current, other );
    assert( _current - _buffer <= _bufferSize );
    value = other;
    return *this;
  }

  /// Decompress an int from the buffer into value
  /// @param value reference to the container for the value.
  RVLDecompressStream& operator>> ( int& value ) {
    _current = RVLCompress::decompress_int( _current, value );
    assert( _current - _buffer <= _bufferSize );    
    return *this;
  }

  /// Decompress an unsigned int from the buffer into value
  /// @param value reference to the container for the value.
  RVLDecompressStream& operator>> ( unsigned int& value ) {
    int v;
    _current = RVLCompress::decompress_int( _current, v );
    value = (unsigned int) v;
    assert( _current - _buffer <= _bufferSize );    
    return *this;
  }

  /// Decompress a float from the buffer into value
  /// @param value reference to the container for the value.
  RVLDecompressStream& operator>> ( float& value ) {
    // doubles aren't compressed
    memcpy( &value, _current, sizeof value );
    _current += sizeof value;
    return *this;
  }
  /// @return true if no more values in the buffer, otherwise false.
  bool done() const {
    return (_current - _buffer) >= _bufferSize;
  }
};

#endif // INDRI_RVLDECOMPRESSSTREAM_HPP
