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
// FieldListDiskBlockWriter.hpp
//
// 5 February 2004 -- tds
//

#ifndef INDRI_KEYFILEFIELDLISTDISKBLOCKWRITER_HPP
#define INDRI_KEYFILEFIELDLISTDISKBLOCKWRITER_HPP

#define INDRI_FIELDLIST_BLOCKSIZE (4096)
#define INDRI_EXTENT_REQUIRED_SPACE (5*3+10+4) // doc id, begin, end, numeric, additional _extentCount room

#include "RVLCompress.hpp"

namespace indri {
  namespace index {
    class FieldListDiskBlockWriter {
    private:
      char _block[ INDRI_FIELDLIST_BLOCKSIZE ];
      char* _data;
      char* _extentCountSpot;

      unsigned int _lastEnd;
      unsigned int _lastDocument;
      unsigned int _extentCount;

      bool _numeric;

      const char* _beginMetadata() const {
        return _block + sizeof _block - 2*sizeof(UINT32);
      }

      void _terminateDocument() {
        if( _extentCount ) {
          int extentCountSize = RVLCompress::compressedSize( _extentCount );

          if( extentCountSize > 1 ) {
            ::memmove( _extentCountSpot + extentCountSize,
                      _extentCountSpot + 1,
                      _data - _extentCountSpot - 1 );
            _data += extentCountSize - 1;
          }

          char* endExtentCount = RVLCompress::compress_int( _extentCountSpot, _extentCount );
          assert( endExtentCount == _extentCountSpot + extentCountSize );
        }

        _extentCount = 0;
        _extentCountSpot = 0;
      }

    public:
      FieldListDiskBlockWriter( bool numeric = false ) {
        clear();
        _numeric = numeric;
      }

      void clear() {
        _lastEnd = 0;
        _lastDocument = 0;
        _extentCount = 0;
        _extentCountSpot = 0;
        _data = _block;
      }

      const char* data() const {
        return _block;
      }

      unsigned int dataSize() const {
        return sizeof _block;
      }

      int lastDocument() {
        return _lastDocument;
      }

      bool addExtent( unsigned int documentID, unsigned int begin, unsigned int end, UINT64 number = 0 ) {
        if( _beginMetadata() - _data < INDRI_EXTENT_REQUIRED_SPACE )
          return false;

        if( documentID != _lastDocument ) {
          _terminateDocument();

          _data = RVLCompress::compress_int( _data, documentID - _lastDocument );
          _lastDocument = documentID;
          _lastEnd = 0;
          _extentCountSpot = _data;
          _extentCount = 0;
          _data++;
        }

        _data = RVLCompress::compress_int( _data, begin - _lastEnd );
        _data = RVLCompress::compress_int( _data, end - begin );

        if( _numeric ) {
          _data = RVLCompress::compress_longlong( _data, number );
        }

        _extentCount++;
        _lastEnd = end;

        assert( begin <= end );
        assert( _data < _beginMetadata() );
        return true;
      }

      void close() {
        char* lastDocumentSpot = _block + INDRI_FIELDLIST_BLOCKSIZE - sizeof(UINT32);
        char* lastByteSpot = _block + INDRI_FIELDLIST_BLOCKSIZE - 2*sizeof(UINT32);

        _terminateDocument();
        *(UINT32*) lastDocumentSpot = _lastDocument;
        *(UINT32*) lastByteSpot = UINT32(_data - _block) | ( _numeric ? 0x80000000 : 0 );
      }
    };
  }
}

#endif // INDRI_KEYFILEFIELDLISTDISKBLOCKWRITER_HPP

