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
// KeyfileFieldListIterator
//
// 10 January 2004 -- tds
//

#ifndef INDRI_FIELDLISTITERATOR_HPP
#define INDRI_FIELDLISTITERATOR_HPP

#include "indri/Extent.hpp"
#include "indri/FieldListDiskBlockReader.hpp"
#include "File.hpp"
#include "ReadBuffer.hpp"

#define INDRI_FIELDLISTITERATOR_BUFFERSIZE (1024*1024)

namespace indri {
  namespace index {

    struct FieldExtentInfo {
      int documentID;
      greedy_vector<Extent> extents;
      greedy_vector<INT64> numbers;
    };

    class FieldListIterator {
    private:
      FieldListDiskBlockReader _reader;
      FieldExtentInfo _info;
      bool _finished;
      bool _hasMore;
      bool _numeric;

      File& _file;
      ReadBuffer _readBuffer;

      bool _fetchNextBlock() {
        const char* nextBlock = _readBuffer.read( INDRI_FIELDLIST_BLOCKSIZE );

        if( !nextBlock )
          return false;

        _reader.setBlock( nextBlock );
        _reader.next();
        return true;
      }

      void _fetchDocument( int documentID, bool& endBlock ) {
        if( _numeric ) {
          _fetchDocumentNumeric( documentID, endBlock );
        } else {
          _fetchDocumentNonNumeric( documentID, endBlock );
        }
      }

      void _fetchDocumentNonNumeric( int documentID, bool& endBlock ) {
        endBlock = false;

        if( documentID != _reader.document() )
          return;

        _info.extents.push_back( _reader.extent() );

        while( _reader.next() ) {
          if( _reader.document() != documentID )
            return;

          _info.extents.push_back( _reader.extent() );
        }

        endBlock = true;
      }

      void _fetchDocumentNumeric( int documentID, bool& endBlock ) {
        endBlock = false;

        if( documentID != _reader.document() )
          return;

        _info.extents.push_back( _reader.extent() );
        _info.numbers.push_back( _reader.number() );

        while( _reader.next() ) {
          if( _reader.document() != documentID )
            return;

          _info.extents.push_back( _reader.extent() );
          _info.numbers.push_back( _reader.number() );
        }

        endBlock = true;
      }

    public:
      FieldListIterator( File& fieldListFile, bool numeric ) :
        _file( fieldListFile ),
        _readBuffer( fieldListFile, INDRI_FIELDLISTITERATOR_BUFFERSIZE, false ),
        _numeric(numeric)
      {
      }

      void startIteration() {
        _hasMore = true;
        _readBuffer.seekg( 0, std::fstream::beg );
        _info.documentID = -1;
        _finished = !_fetchNextBlock();
      }

      bool hasMore() {
        return _hasMore;
      }

      const FieldExtentInfo* currentEntry() {
        if( !_finished )
          return &_info;
        
        return 0;
      }

      const FieldExtentInfo* nextEntry() {
        if( _finished )
          return 0;

        if( !_hasMore ) {
          _finished = true;
          return 0;
        }

        // reader is pointing to next document ID already
        int documentID = _reader.document();
        _info.documentID = documentID;
        _info.extents.clear();
        _info.numbers.clear();
        bool atEndOfBlock;

        _fetchDocument( documentID, atEndOfBlock );

        while( atEndOfBlock ) {
          if( ! _fetchNextBlock() ) {
            _hasMore = false;
            return &_info;
          }

          _fetchDocument( documentID, atEndOfBlock );
        }

        return &_info;
      }

      const FieldExtentInfo* nextEntry( int documentID ) {
        if( _finished )
          return 0;

        if( documentID <= _info.documentID )
          return &_info;

        if( !_hasMore ) {
          _finished = true;
          return 0;
        }

        // we can skip blocks that only have documents that
        // we aren't interested in
        while( _reader.lastDocument() < documentID ) {
          if( ! _fetchNextBlock() ) {
            _finished = true;
            _hasMore = false;
            return 0;
          }
        }

        const FieldExtentInfo* info;

        // read each entry until we find one that matches, or until we're done
        for( info = nextEntry(); info && info->documentID < documentID; info = nextEntry() )
          ;

        return info;
      }
    };
  }
}

#endif // INDRI_FIELDLISTITERATOR_HPP
