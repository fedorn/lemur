/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
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
  // if the doc is not ok, or ok to copy, it
  // will be a document of length 0.
  if( doc->isOk() && doc->okToCopy() ) {
    void* stream = &_documentTextBuffer;
    textOut = new TextOutputDev( buffer_write, stream, gFalse, gFalse);
  
    if ( textOut->isOk() ) {
      int firstPage = 1;
      int lastPage = doc->getNumPages();

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
