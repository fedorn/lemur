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
// FieldListDiskBlockReader.hpp
//
// 10 February 2004 -- tds
//

#ifndef INDRI_KEYFILEFIELDLISTDISKBLOCKREADER_HPP
#define INDRI_KEYFILEFIELDLISTDISKBLOCKREADER_HPP

#include "indri/Extent.hpp"
#include "indri/FieldListDiskBlockWriter.hpp"

class FieldListDiskBlockReader {
private:
  Extent _extent;
  const char* _current;
  const char* _block;
  int _extentCount;
  int _document;
  int _endByte;
  bool _numeric;
  INT64 _number;

public:
  void setBlock( const char* block ) {
    _block = block;
    _current = block;
    _document = 0;
    _extentCount = 0;
    _extent.begin = 0;
    _extent.end = 0;
    _number = 0;

    int endAndNumeric = * (int*) ( _block + INDRI_FIELDLIST_BLOCKSIZE - 2*sizeof(int) );
    _numeric = (endAndNumeric & 0x80000000) ? true : false;
    _endByte = endAndNumeric & 0x7fffffff;
  }

  int lastDocument() {
    return * (int*) ( _block + INDRI_FIELDLIST_BLOCKSIZE - sizeof(int) );
  }

  bool next() {
    if( ! _extentCount ) {
      if( !hasMore() )
        return false;

      int documentDelta;

      _current = RVLCompress::decompress_int( _current, documentDelta );
      _current = RVLCompress::decompress_int( _current, _extentCount );
      _document += documentDelta;
    
      _extent.begin = 0;
      _extent.end = 0;
    }

    int beginDelta;
    int endDelta;

    _current = RVLCompress::decompress_int( _current, beginDelta );
    _current = RVLCompress::decompress_int( _current, endDelta );

    if( _numeric ) {
      _current = RVLCompress::decompress_longlong( _current, _number );
    }

    assert( _current < _block + INDRI_FIELDLIST_BLOCKSIZE );

    _extentCount--;
    _extent.begin = _extent.end + beginDelta;
    _extent.end = _extent.begin + endDelta;
    return true;
  }

  int document() const {
    return _document;
  }

  INT64 number() const {
    return _number;
  }

  const Extent& extent() const {
    return _extent;
  }

  bool hasMore() {
    return _endByte > _current - _block;
  }
};

#endif // INDRI_FIELDLISTDISKBLOCKREADER_HPP
