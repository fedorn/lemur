%option noyywrap
%option never-interactive
%option prefix="parser"

%{

/* Tagged Text Parser for the Indri Project
   By: Don Metzler
*/

#include "indri/TaggedTextParser.hpp"
#include "string-set.h"
  
static long byte_position;
  
%}

%%

\<[^\>]*\>                                    { byte_position +=parserleng; return TaggedTextTokenType::tag; }
[&]([a-zA-Z]+|[#][0-9])[;]                     { byte_position += parserleng; /* zap symbols */ }
[a-zA-Z0-9'\.€-ÿ]+                   { byte_position += parserleng; return TaggedTextTokenType::word; }
[\n]                                           { byte_position += parserleng; /* zap newline */ }
.                                              { byte_position += parserleng; return TaggedTextTokenType::unknown; }

%%

TaggedTextParser::TaggedTextParser() :
  tl(new TagList()),
  _metaList(new TagList()),
  _tagTable(1024),
  _handler(0)
{
}

TaggedTextParser::~TaggedTextParser() {
  delete tl;
  delete _metaList;

  HashTable<const char*, tag_properties*>::iterator iter;
  for( iter = _tagTable.begin(); iter != _tagTable.end(); iter++ ) {
    free( *iter->second );
  }
}

void TaggedTextParser::initialize( UnparsedDocument* document, ParsedDocument* parsed ) {
  _exclude = false;
  _include = _defaultInclude;
  _startIncludeRegion = 0;
  _startExcludeRegion = 0;
  
  tl->clear();
  _metaList->clear();
}

void TaggedTextParser::cleanup( UnparsedDocument* unparsed, ParsedDocument* parsed ) {
  tl->writeTagList( parsed->tags );
  _metaList->writeMetadataList( parsed->metadata, _termBuffer, unparsed->text );
}

TaggedTextParser::tag_properties* TaggedTextParser::_findTag( const char* name ) {
  tag_properties** result = _tagTable.find(name);
  
  if(!result)
    return 0;
  else
    return *result;
}

TaggedTextParser::tag_properties* TaggedTextParser::_buildTag( const std::string& name, const std::map<std::string, std::string>& conflations ) {
  tag_properties* result = 0;

  if( result = _findTag(name.c_str()) )
    return result;

  std::string conflation = name;
  std::map<std::string, std::string>::const_iterator iter;

  iter = conflations.find(name);

  if( iter != conflations.end() )
    conflation = iter->second;

  result = (tag_properties*) malloc( sizeof(tag_properties) + name.length()+1 + conflation.length()+1 );
  result->index = false;
  result->exclude = false;
  result->include = false;
  result->metadata = false;
  char* pName = (char*) result + sizeof(tag_properties);
  char* pConflation = pName + name.length() + 1;
  result->name = pName;
  result->conflation = pConflation;
  strcpy( pName, name.c_str() );
  strcpy( pConflation, conflation.c_str() );
  _tagTable.insert( result->name, result );
  return result;
}

void TaggedTextParser::setTags( const std::vector<std::string>& include,
                                const std::vector<std::string>& exclude,
                                const std::vector<std::string>& index,
                                const std::vector<std::string>& metadata, 
                                const std::map<std::string,std::string>& conflations ) {
  _defaultInclude = true;

  for( size_t i=0; i<include.size(); i++ ) {
    tag_properties* result = _buildTag(include[i], conflations);
    result->include = true;
    _defaultInclude = false;
  }

  for( size_t i=0; i<exclude.size(); i++ ) {
    tag_properties* result = _buildTag(exclude[i], conflations);
    result->exclude = true;
  }

  for( size_t i=0; i<index.size(); i++ ) {
    tag_properties* result = _buildTag(index[i], conflations);
    result->index = true;
  }

  for( size_t i=0; i<metadata.size(); i++ ) {
    tag_properties* result = _buildTag(metadata[i], conflations);
    result->metadata = true;
  }
}

ParsedDocument* TaggedTextParser::parse( UnparsedDocument* document ) {
  _termBuffer.clear();
  _termBuffer.grow( document->textLength * 4 ); // need to leave room here for relative->absolute URL expansion
  _document.terms.clear();
  _document.tags.clear();
  _document.positions.clear();
  
  _document.metadata = document->metadata;
  _document.text = document->text;
  _document.textLength = document->textLength;
  
  byte_position = 0;
  initialize(document, &_document);
  int len = document->textLength;
  parser_scan_bytes(document->text, len);
  doParse();
  cleanup(document, &_document);
  parser_delete_buffer(yy_current_buffer);
  
  return &_document;
}

// lex wrapper
void TaggedTextParser::doParse() {
  int tokenType;

  while(tokenType = parserlex())
    handleToken(parsertext, tokenType, _document.terms.size());
}

void TaggedTextParser::handleTag(char* token, long pos) {
  // extract "element" from tag, stripping off any attributes
  char *e = token;
  // lowercase and remove leading <
  for(char *c = token+1; *c != '\0' && *c != ' ' && *c != '\n' && *c != '\t' && *c != '>'; c++)
    *(e++) = tolower(*c);
  *e = '\0';
  bool atEnd = (*token == '/');
  if(atEnd) token++;
  
  const tag_properties* tagProps = _findTag(token);
  bool oldInclude = _include;
  
  if( tagProps ) { 
    // set _include and _exclude flags appropriately:
    if( atEnd ) {
      if( _exclude ) {
        if( tagProps == _startExcludeRegion ) {
          // this is an end tag, and it matches the start of an exclude region
          _startExcludeRegion = 0;
          _exclude = false;
        }
      } else if( _include && tagProps == _startIncludeRegion ) {
        _startIncludeRegion = 0;
        _include = false;
      }
    } else { // !atEnd
      // if we're in exclude mode, new tags don't matter
      if( ! _exclude ) {
        // not in an exclude
        if( !_include ) {
          // not in included territory
          if( tagProps->include && _startIncludeRegion == 0 ) {
            _startIncludeRegion = tagProps;
            _include = true;
          }
        } else {
          // !_exclude && _include
          if( tagProps->exclude && _startExcludeRegion == 0 ) {
            _startExcludeRegion = tagProps;
            _exclude = true;
          }
        }
      }
    }
    
    // index the tags if necessary
    // this may be an end include tag, so we allow oldInclude
    if( tagProps->index && !_exclude && (_include || oldInclude) ) {
      if( atEnd ) {
        endTag(tagProps->name, tagProps->conflation, pos);
      } else {
        addTag(tagProps->name, tagProps->conflation, pos);
      }
    }
    
    // index metadata if necessary
    if( tagProps->metadata ) {
      if( atEnd ) {
        endMetadataTag(tagProps->name, tagProps->conflation, byte_position-parserleng);
      } else {
        addMetadataTag(tagProps->name, tagProps->conflation, byte_position);
      }
    }
  }
}

void TaggedTextParser::handleToken(char *token, int type, long pos) {
  switch(type) {

  case TaggedTextTokenType::tag:
    {
      handleTag(token, pos);
      break;
    }

 case TaggedTextTokenType::word:
   {
     writeToken(token);
     break;
   }
  }
}

void TaggedTextParser::writeToken(char* token) {
  writeToken(token, byte_position-parserleng, byte_position);
}

void TaggedTextParser::writeToken(char *token, int start, int end) {
  if( _exclude || !_include )
    return;

  int tokenLength = strlen(token);
  
  TermExtent extent;
  extent.begin = start;
  extent.end = end;
  _document.positions.push_back(extent);
  
  char* writeLocation = _termBuffer.write(tokenLength+1);
  strcpy( writeLocation, token );
  _document.terms.push_back( writeLocation );
}

void TaggedTextParser::handle( UnparsedDocument* document ) {
  _handler->handle( parse(document) );
}

void TaggedTextParser::setHandler( ObjectHandler<ParsedDocument>& h ) {
  _handler = &h;
}


