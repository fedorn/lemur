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
// HTMLParser
//
// March 2004 -- metzler
//

#include "indri/TaggedTextParser.hpp"

#ifndef MAX_URL_LENGTH
#define MAX_URL_LENGTH 4096
#endif

class HTMLParser : public TaggedTextParser {
public:
  HTMLParser() {
  }
  
  ~HTMLParser() { }

protected:
  virtual void initialize( UnparsedDocument* unparsed, ParsedDocument* parsed );
  virtual void cleanup( UnparsedDocument* unparsed, ParsedDocument* parsed );
  virtual void handleToken(char *token, int type, long pos);
  char url[MAX_URL_LENGTH];
  char base_url[MAX_URL_LENGTH];
  
  bool normalizeURL(char *s);

private:
  bool extractURL(char *token);
  void parseURL(char *url);
};


