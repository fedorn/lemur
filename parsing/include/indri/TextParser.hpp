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
// TextParser
//
// 16 August 2004 -- tds
//

#ifndef INDRI_TEXTPARSER_HPP
#define INDRI_TEXTPARSER_HPP

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <vector>
#include "indri/IndriParser.hpp"
#include "indri/Buffer.hpp"
#include "string-set.h"

class TextParser : public indri::Parser {
public:
  TextParser();
  ~TextParser();
  
  ParsedDocument* parse( UnparsedDocument* document );

  void handle( UnparsedDocument* document );
  void setHandler( ObjectHandler<ParsedDocument>& h );

  void setTags( const std::vector<std::string>& include,
                const std::vector<std::string>& exclude,
                const std::vector<std::string>& index,
                const std::vector<std::string>& metadata, 
                const std::map<std::string, std::string>& conflations );

protected:
  void writeToken(char* token);
  void writeToken(char *token, int start, int end);
  Buffer _termBuffer;

private:
  ObjectHandler<ParsedDocument>* _handler;
  ParsedDocument _document;
};

#endif // INDRI_TEXTPARSER_HPP

