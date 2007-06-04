/*==========================================================================
 * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

//
// TermRecorder
//
// 14 January 2005 -- tds
//

#include "indri/Buffer.hpp"
#include <vector>
#include <utility>

#ifndef INDRI_TERMRECORDER_HPP
#define INDRI_TERMRECORDER_HPP

namespace indri {
  namespace index {
    class TermRecorder {
    private:
      struct less {
        const char* _base;

        less( const char* base ) {
          _base = base;
        }

        bool operator () ( const std::pair<size_t, int>& one, const std::pair<size_t, int>& two ) {
          return strcmp( _base + one.first, _base + two.first ) < 0;
        }
      };

      indri::utility::Buffer _buffer;
      std::vector< std::pair<size_t, int> > _pairs;

    public:
      void add( int sequence, const char* term ) {
        size_t termLength = strlen( term );
        char* termSpot = _buffer.write( termLength+1 );
        size_t termOffset = termSpot - _buffer.front();
        strcpy( termSpot, term );

        _pairs.push_back( std::make_pair( termOffset, sequence ) );
      }

      void sort() {
        std::sort( _pairs.begin(), _pairs.end(), less( _buffer.front() ) );
      }

      void buildMap( std::vector<int>& map, TermRecorder& other, std::vector< std::pair< const char*, int > >* missing = 0 ) {
        map.resize( _pairs.size(), -1 );
        size_t i = 0;
        size_t j = 0;
        std::vector< std::pair<size_t, int > >& otherPairs = other._pairs;

        // this joins all matching pairs
        while( i < otherPairs.size() && j < _pairs.size() ) {
          int result = strcmp( _buffer.front() + otherPairs[i].first,
                               _buffer.front() + _pairs[j].first );
          
          if( result == 0 ) {
            map[ _pairs[j].second ] = otherPairs[i].second;
            i++;
            j++;
          } else if( result < 0 ) {
            i++;
          } else {
            if( missing )
              missing->push_back( std::make_pair( _buffer.front() + _pairs[j].first, _pairs[j].second ) );

            j++;
          }
        }

        while( missing && j < _pairs.size() ) {
          missing->push_back( std::make_pair( _buffer.front() + _pairs[j].first, _pairs[j].second ) );
          j++;
        }
      }

      std::vector< std::pair<size_t, int> >& pairs() {
        return _pairs;
      }

      indri::utility::Buffer& buffer() {
        return _buffer;
      }

      int memorySize() {
        return int(_buffer.position() + _pairs.size() * sizeof(std::pair<size_t, int>));
      }
    };
  }
}

#endif // INDRI_TERMRECORDER_HPP

