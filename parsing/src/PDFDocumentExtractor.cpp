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
// PDFDocumentExtractor
//
// 25 June 2004 -- tds
//

#include "indri/PDFDocumentExtractor.hpp"
#include "indri/Buffer.hpp"

#include "GString.h"
#include "TextOutputDev.h"
#include "PDFDoc.h"

#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Page.h"
#include "CharTypes.h"
#include "GlobalParams.h"

static void buffer_write( void* stream, char* text, int len ) {
  Buffer* buffer = (Buffer*) stream;

  if( buffer->position() ) {
    buffer->unwrite(1);
  }

  memcpy( buffer->write(len), text, len );
  if( text[len-1] != 0 )
    *buffer->write(1) = 0;
}

PDFDocumentExtractor::PDFDocumentExtractor() {
  globalParams = new GlobalParams(0);
}

PDFDocumentExtractor::~PDFDocumentExtractor() {
  delete globalParams;
  globalParams = 0;
}

void PDFDocumentExtractor::open( const std::string& filename ) {
  _documentTextBuffer.clear();
  _documentPath = filename;
}

void PDFDocumentExtractor::close() {
  _documentPath = "";
}

UnparsedDocument* PDFDocumentExtractor::nextDocument() {
  if( !_documentPath.length() )
    return 0;

  PDFDoc* doc = 0;
  TextOutputDev* textOut = 0;
  GString* gfilename = new GString(_documentPath.c_str());
  doc = new PDFDoc( gfilename );

  int firstPage = 1;
  int lastPage = doc->getNumPages();

  if( doc->isOk() && doc->okToCopy() ) {
    void* stream = &_documentTextBuffer;
    textOut = new TextOutputDev( buffer_write, stream, gFalse, gFalse);
  
    if ( textOut->isOk() ) {
      doc->displayPages(textOut, firstPage, lastPage, 72, 72, 0, gTrue, gFalse);
    }
  }

  delete textOut;
  delete doc;

  _unparsedDocument.textLength = _documentTextBuffer.position();
  char* docnoPoint = _documentTextBuffer.write( _documentPath.length()+1 );
  strcpy( docnoPoint, _documentPath.c_str() );
  _unparsedDocument.text = _documentTextBuffer.front();
  _unparsedDocument.metadata.clear();

  MetadataPair pair;

  pair.key = "path";
  pair.value = docnoPoint;
  pair.valueLength = _documentPath.length()+1;
  _unparsedDocument.metadata.push_back( pair );

  pair.key = "docno";
  pair.value = docnoPoint;
  pair.valueLength = _documentPath.length()+1;
  _unparsedDocument.metadata.push_back( pair );

  _documentPath = "";

  return &_unparsedDocument;
}
