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
// TextDocumentExtractor
//
// 16 August 2004 -- tds
//

#include "indri/TextDocumentExtractor.hpp"
#include "Exception.hpp"

void TextDocumentExtractor::open( const std::string& filename ) {
  _in.clear();
  _in.open( filename.c_str() );
  _filename = filename;

  if( !_in.good() )
    LEMUR_THROW( LEMUR_IO_ERROR, "Couldn't open file " + filename + "." );
}

UnparsedDocument* TextDocumentExtractor::nextDocument() {
  _buffer.clear();
  _document.text = 0;
  _document.textLength = 0;
  _document.metadata.clear();

  if( _in.eof() )
    return 0;

  // set up metadata
  MetadataPair pair;
  pair.value = _filename.c_str();
  pair.valueLength = _filename.length()+1;

  pair.key = "docno";
  _document.metadata.push_back( pair );
  pair.key = "path";
  _document.metadata.push_back( pair );

  pair.key = "filetype";
  pair.value = (void*) "TEXT";
  pair.valueLength = 5;
  _document.metadata.push_back( pair );

  // get document text
  while( !_in.eof() ) {
    int readChunk = 1024*1024;
    char* textSpot = _buffer.write(readChunk);
    _in.read( textSpot, readChunk );
    _buffer.unwrite( readChunk - _in.gcount() );
  }
  *_buffer.write(1) = 0;

  _document.text = _buffer.front();
  _document.textLength = _buffer.position();

  return &_document;
}

void TextDocumentExtractor::close() {
  _in.close();
}
