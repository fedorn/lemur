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
      greedy_vector<UINT64> numbers;
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
