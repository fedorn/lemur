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
