%option noyywrap
%option never-interactive
%option prefix="textparser"

%{
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
// TextParser.lex
//
// Adapted from Don Metzler's TaggedTextParser.lex
//
// 16 August 2004 -- tds
//

#include "indri/TextParser.hpp"
#include "string-set.h"
  
static long byte_position;

%}

%%

[&][a-zA-Z]+[;]                                { byte_position += textparserleng; /* zap symbols */ }
[&][#][0-9]*[;]                                { byte_position += textparserleng; /* zap symbols */ } 
[a-zA-Z0-9]+                                   { byte_position += textparserleng; return 1; }
[A-Z][A-Z]+((\')?[s])?                         { byte_position += textparserleng; return 1; }
[a-zA-Z0-9]+\'[a-zA-Z]+                        { byte_position += textparserleng; return 1;}
[A-Z]\.([A-Z]\.)+                              { byte_position += textparserleng; return 1; }
[\n]                                           { byte_position += textparserleng; /* zap newline */ }
.                                              { byte_position += textparserleng; /* zap unknown char */ }

%%

TextParser::TextParser() :
  _handler(0)
{
}

TextParser::~TextParser() {
}

void TextParser::setTags( const std::vector<std::string>& include,
                          const std::vector<std::string>& exclude,
                          const std::vector<std::string>& index,
                          const std::vector<std::string>& metadata, 
                          const std::map<std::string,std::string>& conflations ) {
  // do nothing
}

ParsedDocument* TextParser::parse( UnparsedDocument* document ) {
  _termBuffer.clear();
  _termBuffer.grow( document->textLength * 2 ); // leave a little extra room
  _document.terms.clear();
  _document.tags.clear();
  _document.positions.clear();
  
  _document.metadata = document->metadata;
  _document.text = document->text;
  _document.textLength = document->textLength;
  
  byte_position = 0;
  int len = document->textLength;
  textparser_scan_bytes(document->text, len);

  while( textparserlex() )
    writeToken( textparsertext );

  textparser_delete_buffer(yy_current_buffer);
  
  return &_document;
}

void TextParser::writeToken(char* token) {
  writeToken(token, byte_position-textparserleng, byte_position);
}

void TextParser::writeToken(char *token, int start, int end) {
  int tokenLength = strlen(token);
  
  TermExtent extent;
  extent.begin = start;
  extent.end = end;
  _document.positions.push_back(extent);
  
  char* writeLocation = _termBuffer.write(tokenLength+1);
  strcpy( writeLocation, token );
  _document.terms.push_back( writeLocation );
}

void TextParser::handle( UnparsedDocument* document ) {
  _handler->handle( parse(document) );
}

void TextParser::setHandler( ObjectHandler<ParsedDocument>& h ) {
  _handler = &h;
}


