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
// DocListDiskBlockReader
//
// 9 January 2004 - tds
//

#ifndef LEMUR_KEYFILEDOCLISTDISKBLOCKREADER_HPP
#define LEMUR_KEYFILEDOCLISTDISKBLOCKREADER_HPP

#define INDRI_DOCLIST_BLOCKSIZE (8192)
#include "RVLCompress.hpp"
#include "indri/DocListInfo.hpp"
#include "indri/DocumentCount.hpp"

namespace indri {
  namespace index {

    class DocListDiskBlockReader {
    private:
      const char* _block;

      const char* _list;
      const char* _listEnd;
      int _termIndex;
      INT16 _termCount;

      bool _finished;
      INT32 _termID;

      int _lastTerm;
      int _firstTerm;
      int _lastDocument;

      struct DocumentData {
        int documentID;
        int positionCount;
        int positions[ INDRI_DOCLIST_BLOCKSIZE ];
      } _documentData;

    public:
      DocListDiskBlockReader() : _finished(true) {
        setBlock(0);
      }

      static int blockSize() {
        return INDRI_DOCLIST_BLOCKSIZE;
      }

      void setBlock( const char* block ) {
        _block = block;

        _termCount = _block ? *(short*) _block : 0;
        
        _termID = 0;
        _list = _block + sizeof(short);
        _listEnd = _list;
        _documentData.documentID = 0;
        _documentData.positionCount = 0;

        const char* endBlock = _block + INDRI_DOCLIST_BLOCKSIZE;

        _lastTerm = (_termCount > 0) ? * (int*) (endBlock - sizeof(int)) : 0;
        _firstTerm = (_termCount > 0) ? * (int*) (endBlock - _termCount*sizeof(int)) : 0;
        _lastDocument = 0;

        if( _termCount > 0 )
          memcpy( &_lastDocument, endBlock - _termCount*(sizeof(int)+sizeof(short)) - sizeof(int), sizeof(int) );

        _termIndex = -1;
        _finished = (_termCount == 0);

        assert( _validateBlock() );
      }

      int lastTerm() {
        return _lastTerm;
      }

      int firstTerm() {
        return _firstTerm;
      }

      int lastDocument() {
        return _lastDocument;
      }

      bool hasMore() {
        return !( (_list == _listEnd) && (_termIndex + 1) == _termCount );
      }

    private:
      bool _trashDocument() {
        _documentData.documentID = MAX_INT32;
        _documentData.positionCount = MAX_INT32;
        _documentData.positions[0] = MAX_INT32;
        return true;
      }

      bool _validateBlock() {
        if( ! _block )
          return true;

        // validate termCount
        short tc = * (short*) _block;
        assert( tc > 0 );
        assert( tc < INDRI_DOCLIST_BLOCKSIZE / sizeof(int) );

        const char* endBlock = _block + INDRI_DOCLIST_BLOCKSIZE;
        UINT32* termIDs = ( (UINT32*) (_block + INDRI_DOCLIST_BLOCKSIZE) ) - _termCount;
        UINT16* offsets = ( (UINT16*) termIDs ) - _termCount;
        UINT32* lastDoc = ( (UINT32*) offsets ) - 1;
        char* beginMetaData = (char*) lastDoc;
        size_t maxDataLength = beginMetaData - _block;

        // validate offsets
        for( int i=0; i<_termCount; i++ ) {
          assert( offsets[i] > sizeof(short) );
          assert( offsets[i] <= maxDataLength );
          if( i>0 )
            assert( offsets[i] > offsets[i-1] );
        }

        // validate termIDs
        for( int i=0; i<_termCount; i++ ) {
          assert( termIDs[i] >= 0 );
          if( i>0 )
            assert( termIDs[i] > termIDs[i-1] );
        }

        // validate lastDoc
        assert( lastDocument() > 0 );
        return true;
      }

      void _skipDocumentLocations() {
        _list = RVLCompress::skip_ints( _list, _documentData.positionCount );
      }
      
      void _fetchDocumentHeader() {
        assert( _list < _listEnd );
        int lastDocument = _documentData.documentID;
        _list = RVLCompress::decompress_int( _list, _documentData.documentID );
        _list = RVLCompress::decompress_int( _list, _documentData.positionCount );
        _documentData.documentID += lastDocument;
        assert( _list + _documentData.positionCount <= _listEnd );
        assert( _documentData.documentID > 0 );
        assert( _documentData.positionCount >= 0 );
      }

      void _fetchDocumentPositions() {
        assert( _list < _listEnd );
        _list = RVLCompress::decompress_int_count( _list, _documentData.positions, _documentData.positionCount );

        for( int i=1; i<_documentData.positionCount; i++ ) {
          _documentData.positions[i] += _documentData.positions[i-1];
        }
        assert( _list <= _listEnd );
      }

      void _fetchDocument() {
        _fetchDocumentHeader();
        _fetchDocumentPositions();
      }

      void _skipToTermIndex( int index ) {
        assert( index < _termCount );
        _termIndex = index;

        const char* endBlock = _block + INDRI_DOCLIST_BLOCKSIZE;
        const char* endEndings = endBlock - _termCount*sizeof(INT32);
        const char* beginEndings = endBlock - _termCount*(sizeof(INT32)+sizeof(INT16));
        INT32* termIDs = (INT32*) endEndings;
        INT16* offsets = (INT16*) beginEndings;

        short endOffset = offsets[ _termIndex ];
        short startOffset;
        if( _termIndex == 0 ) {
          startOffset = sizeof(short);
        } else {
          startOffset = offsets[ _termIndex - 1 ];
        }

        _termID = termIDs[ _termIndex ];
        _list = _block + startOffset;
        _listEnd = _block + endOffset;

        _documentData.documentID = 0;
        _documentData.positionCount = 0;
        
        // validate information
        assert( _list <= _listEnd );
        assert( startOffset > 0 && startOffset < INDRI_DOCLIST_BLOCKSIZE );
        assert( endOffset > startOffset && endOffset < INDRI_DOCLIST_BLOCKSIZE );
        assert( _termID <= lastTerm() );
        assert( _termID >= firstTerm() );
      }

      bool _skipToTerm( int termID ) {
        assert( _termID < termID );

        if( termID > lastTerm() ) {
          assert( _trashDocument() );
          _termIndex = _termCount;
          _finished = true;
          return true;
        }

        // find termID array
        const char* endBlock = _block + INDRI_DOCLIST_BLOCKSIZE;
        const char* beginTermIDs = endBlock - _termCount*sizeof(UINT32);
        UINT32* termIDs = (UINT32*) beginTermIDs;
        UINT32* start = termIDs;
        UINT32* finish = termIDs + _termCount - 1;

        // binary search for the right location  
        while( (finish - start) >= 2 ) {
          UINT32* middle = start + (finish-start)/2;

          if( *middle > unsigned(termID) )
            finish = middle;
          else
            start = middle;
        }
        
        UINT32* termLocation; 

        if( *start >= unsigned(termID) )
          termLocation = start;
        else
          termLocation = finish;

        // the term isn't here!
        if( *termLocation != unsigned(termID) )
          return false;
        
        // now, reset everything
        assert( (termLocation - termIDs) <= _termCount );
        assert( *termLocation == unsigned(termID) );

        _skipToTermIndex( int(termLocation - termIDs) );
        return true;
      }

    public:
      // Returns true if the requested (termID, documentID) pair
      // should be in a block after this one.  This 
      // method is used for skipping--if this returns false,
      // you can safely skip to the next block in search of this
      // pair.
      bool before( int termID, int documentID ) {
        int last = lastTerm();
        int lastDoc = lastDocument();

        // the first termID in this block is after this one
        // so we're definitely behind
        if( last < termID )
          return true;

        // the last termID in this block is this termID,
        // but the last documentID is smaller than the one
        // we want, so we're still smaller
        if( last == termID && lastDoc < documentID )
          return true;

        assert( last > termID || (termID == last && lastDoc >= documentID) );
        return false;
      }

      // Returns true if the requested (termID, documentID) pair
      // should be in this block.  This 
      // method is used for skipping--if this returns false,
      // you can safely skip to the next block in search of this
      // pair.
      bool contains( int termID, int documentID ) {
        int last = lastTerm();
        int first = firstTerm();
        
        // the last term is greater than the ID
        // we're searching for, so the document
        // definitely should be here
        if( termID != last )
          return termID >= first && termID <= last;

        return lastDocument() >= documentID;
      }

      // attempt to skip to (termID, documentID)
      // or to a document greater that documentID
      // but that still shares the same termID.
      // return true if successful.
      bool skip( int termID, int documentID ) {
        // attempt to skip to this term
        if( _termID < termID ) {
          if( !_skipToTerm( termID ) ) {
            return false;
          } else if( _list == _listEnd ) {
            // skip to term may have run us off
            // the end of the block, in that
            // case, return false without fetching
            return false;
          } else {
            // we moved forward to get here, so
            // we'd better fetch a document
            _fetchDocument();
          }
        } else if( _list == _listEnd ) {
          // if we're out of documents, or if we've
          // come to the next term, tell the caller
          // we were unsuccessful
          assert( _trashDocument() );
          return false;
        }

        // we have a good document, so return success
        if( _documentData.documentID >= documentID )
          return true;

        // keep looking! it has to be here somewhere!
        while( _list != _listEnd ) {
          _fetchDocumentHeader();

          if( _documentData.documentID >= documentID ) {
            _fetchDocumentPositions();
            return true;
          }

          _skipDocumentLocations();
        }

        // oh, well. no luck
        assert( _trashDocument() );
        return false;
      }

      void currentDocument( DocListInfo& info ) {
        assert( ! _finished );
        assert( _block != 0 );

        info.setTermID( _termID );
        info.setDocID( _documentData.documentID );
	// dmf FIXME
        info.addPositions((LOC_T*) _documentData.positions, _documentData.positionCount );
      }

      bool nextDocument() {
        if( _finished || _block == 0 ) {
          assert( _trashDocument() );
          _finished = true;
          return false;
        }

        if( _list == _listEnd ) {
          // go to the next term
          _termIndex++;

          if( _termIndex >= _termCount ) {
            assert( _trashDocument() );
            _finished = true;
            return false;
          }

          _skipToTermIndex( _termIndex );
        }

        _fetchDocument();
        return true;
      }

      bool readBlock( int termID, greedy_vector<DocumentCount>& entries ) {
        // skip to this term ID
        if( _termID != termID ) {
          if( !_skipToTerm(termID) ) {
            return false;
          }

          // we may have skipped to the end of the block;
          // in this case return true without fetching anything
          if( _list == _listEnd ) {
            return true;
          }
        }

        // check to see if we need to update the last entry
        DocumentCount pair;
        int lastDocument = 0;
        pair.document = 0;
        pair.count = 0;

        // fetch first document
        _list = RVLCompress::decompress_int( _list, pair.document );
        _list = RVLCompress::decompress_int( _list, pair.count );

        // special handling for the first document; it may be that the last
        // block had only part of the positions for this document
        if( entries.size() && entries.back().document == pair.document ) {
          entries.back().count += pair.count;
        } else {
          entries.push_back( pair );
        }

        _list = RVLCompress::skip_ints( _list, pair.count );
        lastDocument = pair.document;

        while( _list < _listEnd ) {
          _list = RVLCompress::decompress_int( _list, pair.document );
          _list = RVLCompress::decompress_int( _list, pair.count );
          _list = RVLCompress::skip_ints( _list, pair.count );
          pair.document += lastDocument;
          lastDocument = pair.document;

          entries.push_back( pair );
        }

        return true;
      }

      bool finished() {
        return _finished;
      }

      int document() const {
        return _documentData.documentID;
      }

      int term() const {
        return _termID;
      }
    };
  }
}

#endif // LEMUR_KEYFILEDOCLISTDISKBLOCKREADER_HPP


