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

\<[^\>]*\>                                    { byte_position +=parserleng; return indri::parse::TaggedTextTokenType::tag; }
[&]([a-zA-Z]+|[#][0-9])[;]                     { byte_position += parserleng; /* zap symbols */ }
[a-zA-Z0-9'\.€-ÿ]+                   { byte_position += parserleng; return indri::parse::TaggedTextTokenType::word; }
"-"[0-9]+(\.[0-9]+)?                   { byte_position += parserleng; return indri::parse::TaggedTextTokenType::word;  /*negative number*/}
[\n]                                           { byte_position += parserleng; /* zap newline */ }
.                                              { byte_position += parserleng; return indri::parse::TaggedTextTokenType::unknown; }

%%

indri::parse::TaggedTextParser::TaggedTextParser() :
  tl(new TagList()),
  _metaList(new TagList()),
  _tagTable(1024),
  _handler(0)
{
}

indri::parse::TaggedTextParser::~TaggedTextParser() {
  delete tl;
  delete _metaList;

  indri::utility::HashTable<const char*, tag_properties*>::iterator iter;
  for( iter = _tagTable.begin(); iter != _tagTable.end(); iter++ ) {
    free( *iter->second );
  }
}

void indri::parse::TaggedTextParser::initialize( indri::parse::UnparsedDocument* document, indri::api::ParsedDocument* parsed ) {
  _exclude = false;
  _include = _defaultInclude;
  _startIncludeRegion = 0;
  _startExcludeRegion = 0;
  
  tl->clear();
  _metaList->clear();
}

void indri::parse::TaggedTextParser::cleanup( indri::parse::UnparsedDocument* unparsed, indri::api::ParsedDocument* parsed ) {
  tl->writeTagList( parsed->tags );
  _metaList->writeMetadataList( parsed->metadata, _termBuffer, unparsed->text );
}

indri::parse::TaggedTextParser::tag_properties* indri::parse::TaggedTextParser::_findTag( const char* name ) {
  tag_properties** result = _tagTable.find(name);
  
  if(!result)
    return 0;
  else
    return *result;
}

indri::parse::TaggedTextParser::tag_properties* indri::parse::TaggedTextParser::_buildTag( const std::string& name, const std::map<std::string, std::string>& conflations ) {
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

void indri::parse::TaggedTextParser::setTags( const std::vector<std::string>& include,
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

indri::api::ParsedDocument* indri::parse::TaggedTextParser::parse( indri::parse::UnparsedDocument* document ) {
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
void indri::parse::TaggedTextParser::doParse() {
  int tokenType;

  while(tokenType = parserlex()) {
    if( indri::parse::TaggedTextTokenType::word == tokenType ) {
      writeToken(parsertext, byte_position-parserleng, byte_position);
    } else if( indri::parse::TaggedTextTokenType::tag == tokenType ) {
      handleTag(parsertext, _document.terms.size());
    }
  }
}

void indri::parse::TaggedTextParser::handleTag(char* token, long pos) {
  // extract "element" from tag, stripping off any attributes
  char *e = token;
  // lowercase and remove leading <
  for(char *c = token+1; *c != '\0' && *c != ' ' && *c != '\n' && *c != '\t' && *c != '>'; c++) {
    if( *c >= 'A' && *c <= 'Z' )
      *(e++) = *c + ('a' - 'A');
    else
      *(e++) = *c;;
  }
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

void indri::parse::TaggedTextParser::handleToken(char *token, int type, long pos) {
  if( indri::parse::TaggedTextTokenType::word == type ) {
    writeToken(token, byte_position-parserleng, byte_position);
  } else if( indri::parse::TaggedTextTokenType::tag == type ) {
    handleTag(token, pos);
  }
}

void indri::parse::TaggedTextParser::writeToken(char* token) {
  writeToken(token, byte_position-parserleng, byte_position);
}

void indri::parse::TaggedTextParser::writeToken(char *token, int start, int end) {
  if( _exclude || !_include )
    return;

  int tokenLength = strlen(token);
  
  TermExtent extent;
  extent.begin = start;
  extent.end = end;
  _document.positions.push_back(extent);
  
  char* writeLocation = _termBuffer.write(tokenLength+1);
  memcpy( writeLocation, token, tokenLength+1 );
  _document.terms.push_back( writeLocation );
}

void indri::parse::TaggedTextParser::handle( indri::parse::UnparsedDocument* document ) {
  _handler->handle( parse(document) );
}

void indri::parse::TaggedTextParser::setHandler( ObjectHandler<indri::api::ParsedDocument>& h ) {
  _handler = &h;
}


