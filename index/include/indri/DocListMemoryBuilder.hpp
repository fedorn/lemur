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
// DocListMemoryBuilder.hpp
//
// tds - 17 December 2003
//

#ifndef LEMUR_KEYFILEDOCLISTMEMORYBUILDER_HPP
#define LEMUR_KEYFILEDOCLISTMEMORYBUILDER_HPP

#include "RVLCompress.hpp"
#include <vector>
#include <assert.h>
#include "indri/greedy_vector"

namespace indri {
  namespace index {
    class DocListMemoryBuilderIterator {
      const greedy_vector< std::pair<char*, char*>, 4 >& _lists;
      greedy_vector< std::pair<char*, char*>, 4 >::const_iterator _current;

      const char* _list;
      const char* _listEnd;

      int _currentDocument;
      int _currentPosition;
      int _positionsLeft;

    public:
      DocListMemoryBuilderIterator( const greedy_vector< std::pair<char*,char*>, 4 >& lists ) :
        _lists(lists)
      {
        _current = _lists.begin();
        _currentDocument = 0;
        _currentPosition = 0;
        _positionsLeft = 0;
        _list = 0;
        _listEnd = 0;

        if( _current != _lists.end() ) {
          _list = _current->first;
          _listEnd = _current->second;
        }
      }

      bool next() {
        if( _list < _listEnd ) {
          if( _positionsLeft > 0 ) {
            // we have more positions left in the current document
            int deltaPosition;
            _list = RVLCompress::decompress_int( _list, deltaPosition );
            _currentPosition += deltaPosition;
            _positionsLeft--;
            return true;
          } else {
            // no positions left, but we have more documents to read
            int deltaDocument;
            _list = RVLCompress::decompress_int( _list, deltaDocument );
            _list = RVLCompress::decompress_int( _list, _positionsLeft );
            _list = RVLCompress::decompress_int( _list, _currentPosition );
            _currentDocument += deltaDocument;
            _positionsLeft--;
            return true;
          }
        } else {    
          assert( _list == _listEnd );

          // no data left, go to the next segment
          if( _current != _lists.end() )
            _current++;
          
          if( _current != _lists.end() ) {
            _list = _current->first;
            _listEnd = _current->second;
            return next();
          }

          // no more list segments
          return false;
        }
      }

      int document() {
        return _currentDocument;
      }

      int position() {
        return _currentPosition;
      }
    };

    class DocListMemoryBuilder {
    public:
      typedef DocListMemoryBuilderIterator iterator;

    private:
      int _documentFrequency;
      int _termFrequency;

      greedy_vector< std::pair<char*,char*>, 4 > _lists;

      char* _list;
      char* _listBegin;
      char* _listEnd;
      char* _locationCountPointer;

      int _lastLocation;
      int _lastDocument;
      int _lastTermFrequency;

      void _storeCompressedInt( std::vector<char>& destination, int data, int previous = 0 );
      void _createDocument( int docID );
      void _writeLocation( int location );
      void _terminateDocument();
      void _terminateSegment();
      void _grow();
      void _copy( DocListMemoryBuilder& other );

    public:
      DocListMemoryBuilder();
      const DocListMemoryBuilder& operator=( DocListMemoryBuilder& other );
      void addLocation( int docID, int location );
      void clear();
      void close();
      iterator getIterator();
      bool empty();

      int documentFrequency() const;
      int termFrequency() const;
      size_t memorySize() const;
      int curDocID() const;
    };
  }
}

#endif // LEMUR_DOCLISTMEMORYBUILDER_HPP

