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
// DocumentIteratorFactory
//
// 5 August 2004 -- tds
//

#include "indri/DocumentIteratorFactory.hpp"

#include "indri/PDFDocumentExtractor.hpp"
#include "indri/TaggedDocumentIterator.hpp"
#include "indri/TextDocumentExtractor.hpp"

#ifdef WIN32
// put in indri or indri/win?
#include "indri/WordDocumentExtractor.hpp"
#include "indri/PowerPointDocumentExtractor.hpp"
#endif

#include <iostream>
#include <locale>
#include "Exception.hpp"

#define TYPE_TAGGED   ( "Tagged Document Collection" )
#define TYPE_PDF      ( "Adobe PDF" )
#define TYPE_WORD     ( "Microsoft Word" )
#define TYPE_PPT      ( "Microsoft PowerPoint" )
#define TYPE_TEXT     ( "Text" )


DocumentIterator* DocumentIteratorFactory::get( const std::string& type, const char* startDocTag, const char* endDocTag, const char* startMetadataTag ) {
  std::string preferred = preferredName( type );
  DocumentIterator* result = 0;

  if( preferred == TYPE_TAGGED ) {
    TaggedDocumentIterator* iter = new TaggedDocumentIterator();
    iter->setTags( startDocTag, endDocTag, startMetadataTag );
    result = iter;
  } else if( preferred == TYPE_PDF ) {
    result = new PDFDocumentExtractor();
  } else if( preferred == TYPE_TEXT ) {
    result = new TextDocumentExtractor();
  }
  
#ifdef WIN32
  else if( preferred == TYPE_WORD ) {
    result = new WordDocumentExtractor();
  } else if( preferred == TYPE_PPT ) {
    result = new PowerPointDocumentExtractor();
  }
#endif

  if( !result )
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, type + " is an unknown DocumentIterator type." );

  return result;
}

DocumentIterator* DocumentIteratorFactory::get( const std::string& type ) {
  return get(type, 0, 0, 0);
}

std::string DocumentIteratorFactory::preferredName( const std::string& type ) {
  if( type == "tagged" || type == TYPE_TAGGED ) {
    return TYPE_TAGGED;
  } else if( type == "pdf" || type == "adobe pdf" || type == TYPE_PDF ) {
    return TYPE_PDF;
  } else if( type == "doc" || type == "msword" || type == "word" || type == "microsoft word" || type == TYPE_WORD ) {
    return TYPE_WORD;
  } else if( type == "ppt" || type == "powerpoint" || type == "msppt" || type == "microsoft powerpoint" || type == TYPE_PPT ) {
    return TYPE_PPT;
  } else if( type == "txt" || type == "text" || type == TYPE_TEXT ) {
    return TYPE_TEXT;
  }

  return "";
}
