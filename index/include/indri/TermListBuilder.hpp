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
// indri::index::TermListBuilder
//
// 14 July 2004 -- tds 
//

#ifndef INDRI_TERMLISTBUILDER_HPP
#define INDRI_TERMLISTBUILDER_HPP

#include "indri/FieldExtent.hpp"
#include "indri/Buffer.hpp"
#include "RVLCompress.hpp"
#include "indri/RVLDecompressStream.hpp"
#include "indri/FieldExtent.hpp"
#include "indri/greedy_vector"

namespace indri {
  namespace index {
    class TermListBuilder {
    private:
      greedy_vector<int> _terms;
      greedy_vector<FieldExtent> _fields;

    public:
      void clear() {
        _terms.clear();
        _fields.clear();
      }

      void addField( const indri::index::FieldExtent& field ) {
        _fields.push_back( field );
      }

      void addTerm( const int termID ) {
        _terms.push_back( termID );
      }

      greedy_vector<int>& terms() {
        return _terms;
      }

      const greedy_vector<int>& terms() const {
        return _terms;
      }

      greedy_vector<indri::index::FieldExtent>& fields() {
        return _fields;
      }

      const greedy_vector<indri::index::FieldExtent>& fields() const {
        return _fields;
      }

      void read( const char* buffer, int size ) {
        clear();
        RVLDecompressStream stream( buffer, size );

        int termCount;
        int fieldCount;

        stream >> termCount
              >> fieldCount;

        for( int i=0; i<termCount; i++ ) {
          int termID;
          stream >> termID;
          _terms.push_back( termID ); 
        }

        for( int i=0; i<fieldCount; i++ ) {
          FieldExtent extent;

          stream >> extent.id
                >> extent.begin
                >> extent.end
                >> extent.number;

          _fields.push_back( extent );
        }
      }

      void write( Buffer& buffer ) {
        // format:
        //   term count
        //   field count
        //   termID * termCount (compressed)
        //   ( fieldID, begin, (delta begin) end, number ) * fieldCount

        buffer.clear();
        int length = 10 + 5 * _terms.size() + 2 * sizeof(FieldExtent) * _fields.size();
        char* begin = buffer.write( length );
        char* out = begin;

        // write count of terms and fields in the document first
        out = RVLCompress::compress_int( out, _terms.size() );
        out = RVLCompress::compress_int( out, _fields.size() );

        // write out terms
        int termsSize = RVLCompress::compress_ints( &_terms.front(), (unsigned char*) out, _terms.size() );
        out += termsSize;

        // write out fields
        for( unsigned int i=0; i<_fields.size(); i++ ) {
          out = RVLCompress::compress_int( out, _fields[i].id );
          out = RVLCompress::compress_int( out, _fields[i].begin );
          out = RVLCompress::compress_int( out, _fields[i].end );
          out = RVLCompress::compress_longlong( out, _fields[i].number );
        }

        buffer.unwrite( length - (out - begin) );
      }
    };
  }
}

#endif // INDRI_TERMLISTBUILDER_HPP
