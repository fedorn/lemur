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
// TaggedTextParser
//
// March 2004 -- metzler
//

#ifndef INDRI_TAGGEDTEXTPARSER_HPP
#define INDRI_TAGGEDTEXTPARSER_HPP

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include "indri/HashTable.hpp"
#include "indri/TagList.hpp"
#include "indri/IndriParser.hpp"
#include "indri/Buffer.hpp"
#include "string-set.h"

#define MAX_DOCNO_LENGTH 128
#define PARSER_MAX_BUF_SIZE 1024

class StringHash {
public:
  int operator() (const std::string key) const {
    int hash = 0;
    for(unsigned int i = 0; i < key.length(); i++)
      hash += (unsigned char)key[i];
    return hash;
  }
};

class StringComparator {
public:
  int operator() (const std::string one, const std::string two) const {
    return one.compare(two);
  }
};

class TaggedTextParser : public indri::Parser {
public:
  TaggedTextParser();
  ~TaggedTextParser();
  
  void setTags( const std::vector<std::string>& include,
                const std::vector<std::string>& exclude,
                const std::vector<std::string>& index,
                const std::vector<std::string>& metadata, 
                const std::map<std::string,std::string>& conflations );

  ParsedDocument* parse( UnparsedDocument* document );

  void handle( UnparsedDocument* document );
  void setHandler( ObjectHandler<ParsedDocument>& h );

protected:
  typedef HashTable<std::string, std::string, StringHash, StringComparator> StrHashTable;

  virtual void handleToken(char *token, int type, long pos);
  virtual void initialize( UnparsedDocument* unparsed, ParsedDocument* parsed );
  virtual void cleanup( UnparsedDocument* unparsed, ParsedDocument* parsed );

  void addTag(const char *s, const char* c, int pos) { tl->addTag(s, c, pos); }
  void endTag(const char *s, const char* c, int pos) { tl->endTag(s, c, pos); }

  void addMetadataTag(const char* s, const char* c, int pos) { _metaList->addTag(s, c, pos); }
  void endMetadataTag(const char* s, const char* c, int pos) { _metaList->endTag(s, c, pos); }

  // tag list
  TagList* tl;
  TagList* _metaList;
  Buffer _termBuffer;

  void writeToken(char *token);

  struct tag_properties {
    const char* name;
    const char* conflation;
    bool index;
    bool exclude;
    bool include;
    bool metadata;
  };
  tag_properties* _findTag(const char* name);
  tag_properties* _buildTag( const std::string& name, const std::map<std::string,std::string>& conflations );
  HashTable<const char*, tag_properties*> _tagTable;
  void handleTag(char* token, long pos);

  const tag_properties* _startExcludeRegion;
  const tag_properties* _startIncludeRegion;
  
  bool _exclude;
  bool _include;
  bool _defaultInclude;
  
private:
  ObjectHandler<ParsedDocument>* _handler;
  ParsedDocument _document;

  void doParse();
  void writeToken(char *token, int start, int end);
  char start_tag[PARSER_MAX_BUF_SIZE];
  char end_tag[PARSER_MAX_BUF_SIZE];
};

namespace TaggedTextTokenType {
  const int tag = 1;
  const int upword = 2;
  const int word = 3;
  const int contraction = 4;
  const int acronym = 5;
  const int acronym2 = 6;
  const int unknown = 7;
};

#endif // INDRI_TAGGEDTEXTPARSER_HPP





