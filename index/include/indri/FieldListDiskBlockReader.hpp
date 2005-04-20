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
// FieldListDiskBlockReader.hpp
//
// 10 February 2004 -- tds
//

#ifndef INDRI_KEYFILEFIELDLISTDISKBLOCKREADER_HPP
#define INDRI_KEYFILEFIELDLISTDISKBLOCKREADER_HPP

#include "indri/Extent.hpp"
#include "indri/FieldListDiskBlockWriter.hpp"

namespace indri {
  namespace index {
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

          if( _number & 1 ) {
            // number is negative
            _number = -((_number + 1) / 2);
          } else {
            // number is positive
            _number = _number / 2;
          }
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

      const indri::index::Extent& extent() const {
        return _extent;
      }

      bool hasMore() {
        return _endByte > _current - _block;
      }
    };
  }
}

#endif // INDRI_FIELDLISTDISKBLOCKREADER_HPP
