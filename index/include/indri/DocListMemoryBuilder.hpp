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

