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
// TermData.hpp
//
// 4 February 2004 -- tds
//

#ifndef INDRI_TERMDATA_HPP
#define INDRI_TERMDATA_HPP

#include "indri/TermFieldStatistics.hpp"
#include <indri/greedy_vector>
#include "indri/DocListMemoryBuilder.hpp"
#include "File.hpp"
#include "lemur-compat.hpp"
#include "indri/RVLCompressStream.hpp"
#include "indri/RVLDecompressStream.hpp"

#ifdef WIN32
// remove warning about zero-sized arrays
#pragma warning ( disable: 4200 )
#endif 

#define INDRI_MAX_SEGMENTS (8)

namespace indri {
  namespace index {
    struct TermData {
    private:
      // these are private, bogus functions so that this object can never be copied
      // we don't want to be able to copy it, because any real copy operator needs to
      // take into account the data in the fields[] array, and we don't know how long it is.
      TermData( const TermData& other ) {}
      const TermData& operator= ( const TermData& other ) { return *this; }

    public:
      TermData() :
          maxDocumentFrequency(0),
          maxDocumentFraction(0),
          minDocumentLength(MAX_INT32)
      {
        term = 0;
        
        memset( segmentOffsets, 0xFF, sizeof segmentOffsets );
      }

      File::offset_type segmentOffsets[INDRI_MAX_SEGMENTS];
      TermFieldStatistics corpus;
      DocListMemoryBuilder list;

      float maxDocumentFraction;         // argmax_documents of (termCount/docLength)
      unsigned int maxDocumentFrequency; // maximum number of times this term appears in any given document
      unsigned int minDocumentLength;    // minimum length of any document that contains this term

      const char* term;                  // name of this term

      TermFieldStatistics fields[0];
    };
  }
}

inline indri::index::TermData* termdata_create( int fieldCount ) {
  // allocate enough room for the term data, plus enough room for fields
  void* buffer = malloc( sizeof(indri::index::TermData) + sizeof(indri::index::TermFieldStatistics)*fieldCount );
  
  // call the constructor in place
  new(buffer) indri::index::TermData();

  // call field data constructors in place
  for( int i=0; i<fieldCount; i++ ) {
    new((char*)buffer +
        sizeof(indri::index::TermData) +
        sizeof(indri::index::TermFieldStatistics)*i) indri::index::TermFieldStatistics();
  }

  return (indri::index::TermData*) buffer;
}

inline void termdata_delete( indri::index::TermData* termData, int fieldCount ) {
  if( termData ) {
    termData->~TermData();

    for( int i=0; i<fieldCount; i++ ) {
      termData->fields[i].~TermFieldStatistics();
    }

    free(termData);
  }
}

inline int termdata_size( int fieldCount ) {
  return sizeof(indri::index::TermData) + fieldCount * sizeof(indri::index::TermFieldStatistics);
}

inline int termdata_compress( char* buffer, int size, int fieldCount, indri::index::TermData* termData ) {
  RVLCompressStream stream( buffer, size );

  // corpus statistics
  stream << termData->corpus.totalCount
         << termData->corpus.documentCount;

  // max-score statistics
  stream << termData->maxDocumentFrequency
         << termData->minDocumentLength
         << termData->maxDocumentFraction;

  // segment information
  
  int numSegments = 0;

  // count up the number of segments used here
  for( size_t i=0; i<INDRI_MAX_SEGMENTS; i++ ) {
    if( termData->segmentOffsets[i] != MAX_INT64 )
      numSegments++;
  }

  stream << numSegments;

  // stream out only the segment offsets that are used
  for( unsigned int i=0; i<INDRI_MAX_SEGMENTS; i++ ) {
    if( termData->segmentOffsets[i] != MAX_INT64 ) {
      stream << i
            << termData->segmentOffsets[i];
    }
  }

  // field statistics
  for( int i=0; i<fieldCount; i++ ) {
    stream << termData->fields[i].totalCount
          << termData->fields[i].documentCount;
  }

  return stream.dataSize();
}

inline void termdata_decompress( const char* buffer, int size, int fieldCount, indri::index::TermData* termData ) {
  RVLDecompressStream stream( buffer, size );
  
  // corpus statistics
  stream >> termData->corpus.totalCount
        >> termData->corpus.documentCount;

  // max-score statistics
  stream >> termData->maxDocumentFrequency
        >> termData->minDocumentLength
        >> termData->maxDocumentFraction;

  // segment information
  int numSegments = 0;
  stream >> numSegments;

  for( int i=0; i<numSegments; i++ ) {
    int segment;
    File::offset_type offset;

    stream >> segment
          >> offset;

    termData->segmentOffsets[segment] = offset;
  }

  // field statistics
  for( int i=0; i<fieldCount; i++ ) {
    stream >> termData->fields[i].totalCount
          >> termData->fields[i].documentCount;
  }
}

#endif // INDRI_TERMDATA_HPP
