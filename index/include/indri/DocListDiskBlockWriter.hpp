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
// DocListDiskBlockWriter
// 
// 9 January 2004 - tds
//

#ifndef INDRI_DOCLISTDISKBLOCKWRITER_HPP
#define INDRI_DOCLISTDISKBLOCKWRITER_HPP

#include "indri/DocListDiskBlockReader.hpp"
#include <indri/greedy_vector>

// keep enough room for a new termID, new ending, new docID, new location count, new position
#define INDRI_MAX_SINGLE_DOC_POSITION_SIZE (4+2+5*3)
#define INDRI_MAX_POSITION_SIZE (5)

namespace indri {
  namespace index {
    class DocListDiskBlockWriter {
      char _block[ INDRI_DOCLIST_BLOCKSIZE ];
      char* _data;
      char* _positionsSpot;
      int _positions;
      greedy_vector<UINT32> _termIDs;
      greedy_vector<UINT16> _endings;

      int _lastDocumentID;
      int _lastTermID;
      int _lastPosition;
    
    public:
      DocListDiskBlockWriter() {
        clear();
      }

    private:
      char* _beginMetadata() {
        size_t terms = _termIDs.size();
        size_t currentTermSize = terms * sizeof(UINT32);
        size_t currentEndingsSize = terms * sizeof(UINT16);
        size_t lastDocumentIDSize = sizeof(UINT32);

        return _block + INDRI_DOCLIST_BLOCKSIZE - (currentTermSize + currentEndingsSize + lastDocumentIDSize);
      }

      void _completeTerm() {
        // _addPositionsCount has to come before endings, because
        // _addPositionsCount may change the value of _data
        _addPositionsCount();
        _endings.push_back( (UINT16) (_data - _block) );
      }

      void _addPositionsCount() {
        assert( _positionsSpot );
        int compressedSize = RVLCompress::compressedSize( _positions );

        if( compressedSize > 1 ) {
          ::memmove( _positionsSpot + compressedSize, _positionsSpot + 1, _data - _positionsSpot - 1 );
          _data += compressedSize - 1;
        }

        RVLCompress::compress_int( _positionsSpot, _positions );
        _positions = 0;
        _positionsSpot = 0;
      }

      void _addTerm( int termID ) {
        if( _termIDs.size() ) {
          _completeTerm();
        }

        _termIDs.push_back( termID );
        _lastTermID = termID;
        _lastDocumentID = 0;
        _lastPosition = 0;
      }

      void _addDocument( int documentID ) {
        if( _positions ) {
          _addPositionsCount();
        }

        _data = RVLCompress::compress_int( _data, documentID - _lastDocumentID );
        _lastDocumentID = documentID;
        _lastPosition = 0;

        // leave one byte around for positions
        _positionsSpot = _data;
        _data += 1;
      }

      void _addPosition( int position ) {
        _data = RVLCompress::compress_int( _data, position - _lastPosition );
        _lastPosition = position;
        _positions++;
      }

    public:
      int addDocument( int termID, int documentID, const int* positions, int positionCount ) {
        int i;

        if( _beginMetadata() - _data < INDRI_MAX_SINGLE_DOC_POSITION_SIZE )
          return 0;
        
        if( termID != _lastTermID )
          _addTerm( termID );
      
        _addDocument( documentID );

        char* metadata = _beginMetadata();

        for( i=0; i<positionCount && metadata - _data > INDRI_MAX_POSITION_SIZE; i++ ) {
          _addPosition( positions[i] );
        }
        
        return i;
      }

      bool addPosition( int termID, int documentID, int position ) {
        if( _beginMetadata() - _data < INDRI_MAX_SINGLE_DOC_POSITION_SIZE )
          return false;
            
        if( termID != _lastTermID ) {
          _addTerm( termID );
        }

        if( documentID != _lastDocumentID ) {
          _addDocument( documentID );
        }

        _addPosition( position );
        return true;
      }

      void close() {
        //
        // This method adds all the metadata to the block and prepares
        // it for writing to disk.  The block format is as follows:
        //
        // Header:  2-byte termCount
        // Data:    Variable length section consisting of documentIDs and 
        //          locations, compressed
        // Void:    possibly some unused data space here
        // Footer:  4-byte last documentID
        //          (2 byte * termCount) endings -- the offsets into the
        //              data portion where the termID changes
        //          (4 byte * termCount) termIDs -- termIDs used in this
        //              block

        if( _positions ) {
          _completeTerm();
        }

        assert( _endings.size() == _termIDs.size() );

        // put a term count at the beginning of the block
        UINT16 termCount = UINT16(_termIDs.size());
        memcpy( _block, &termCount, sizeof(termCount) );

        char* endBlock = _block + sizeof(_block);
        char* startTermIDs = endBlock - sizeof(UINT32)*termCount;
        char* startEndings = startTermIDs - sizeof(UINT16)*termCount;
        char* startFinalDocID = startEndings - sizeof(UINT32);

        assert( startFinalDocID >= _data ); // term list data and end of block data shouldn't overlap

        // store the termID list at the end of the block
        memcpy( startTermIDs, &_termIDs.front(), termCount*sizeof(UINT32) );

        // store endings
        memcpy( startEndings, &_endings.front(), termCount*sizeof(UINT16) );

        // store final document ID
        memcpy( startFinalDocID, &_lastDocumentID, sizeof(UINT32) );
      }

      void clear() {
        _termIDs.clear();
        _endings.clear();
        _data = _block + sizeof(UINT16);

        _lastDocumentID = 0;
        _lastTermID = 0;
        _lastPosition = 0;
        _positions = 0;
        _positionsSpot = 0;
      }

      char* data() {
        return _block;
      }

      int dataSize() const {
        return INDRI_DOCLIST_BLOCKSIZE;
      }
    };
  }
}

#endif // INDRI_DOCLISTDISKBLOCKWRITER_HPP

