%option noyywrap
%option never-interactive
%option prefix="textparser"
%{

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


