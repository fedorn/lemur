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
// Buffer
//
// 14 May 2004 -- tds
//

#ifndef INDRI_BUFFER_HPP
#define INDRI_BUFFER_HPP

#include <assert.h>
#include "lemur-compat.hpp"

class Buffer {
private:
  char* _buffer;
  size_t _size;
  size_t _position;

public:
  Buffer() :
    _buffer(0),
    _size(0),
    _position(0)
  {
  }

  ~Buffer() {
    free( _buffer );
  }

  size_t size() const {
    return _size;
  }

  size_t position() const {
    return _position;
  }

  void clear() {
    _position = 0;
  }

  char* front() {
    return _buffer;
  }

  char* write( size_t length ) {
    grow( _position + length );
    char* spot = _buffer + _position;
    _position += length;
    return spot;
  }

  void unwrite( size_t length ) {
    assert( length <= _position );
    _position -= length;
  }
  
  void grow( size_t newSize ) {
    if( newSize > _size ) {
      if( newSize < 1024*1024 ) {
        // find next larger power of two, up to one megabyte
        size_t powSize;
        for( powSize = 64; powSize < newSize; powSize *= 2 )
          ;
        newSize = powSize;
      } else {
        // round to nearest megabyte
        newSize = (newSize + 1024*1024) & ~(1024*1024-1);
      }

      char* newBuffer = (char*) malloc( newSize );
      memcpy( newBuffer, _buffer, _position );
      free( _buffer );
      _buffer = newBuffer;
      _size = newSize;
    }
  }

  void grow() {
    if( _size == 0 )
      grow(64);
    else
      grow(_size*2);
  }

  void remove( size_t start ) {
    memmove( _buffer, _buffer + start, _position - start );
    _position -= start;
  }

  void detach() {
    _size = 0;
    _buffer = 0;
  }
};

#endif // INDRI_BUFFER_HPP
