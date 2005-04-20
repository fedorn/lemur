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
// FileClassEnvironmentFactory
//
// 23 August 2004 -- tds
//

#include "indri/FileClassEnvironmentFactory.hpp"
#include "indri/DocumentIteratorFactory.hpp"
#include "indri/ParserFactory.hpp"
#include <map>
#include <vector>

struct file_class_environment_spec {
  const char* name;
  const char* parser;
  const char* iterator;

  // iterator tag information
  const char* startDocTag;
  const char* endDocTag;
  const char* endMetadataTag;

  // parser tag information
  const char** includeTags;
  const char** excludeTags;
  const char** indexTags;
  const char** metadataTags;
  const char** conflations;
};


//
// Preconfigured environments
//

static const char* html_index_tags[] = { "title", "h1", "h2", "h3", "h4", 0 };
static const char* html_metadata_tags[] = { "title", 0 };
static const char* html_conflations[] = { "h1", "heading", "h2", "heading", "h3", "heading", "h4", "heading", 0, 0 };

static const char* trec_include_tags[] = { "text", "hl", "head", "headline", "title", "ttl", "dd", "date", "lp", "leadpara", 0 };
static const char* trec_metadata_tags[] = { "docno", "title", 0 };
static const char* trec_conflations[] = { "hl", "headline", "head", "headline", "ttl", "title", "dd", "date", 0, 0 };
static const char* trec_index_tags[] = { "hl", "head", "headline", "title", "ttl", "dd", "date", 0 };

static file_class_environment_spec environments[] = {
  { 
    "html",               // name
    "html",               // parser
    "text",               // iterator
    NULL,                 // startDocTag
    NULL,                 // endDocTag
    NULL,                 // endMetadataTag
    NULL,                 // includeTags
    NULL,                 // excludeTags
    html_index_tags,      // indexTags
    html_metadata_tags,   // metadataTags
    html_conflations      // conflations
  },
  {
    "xml",               // name
    "xml",               // parser
    "text",               // iterator
    NULL,                 // startDocTag
    NULL,                 // endDocTag
    NULL,                 // endMetadataTag
    NULL,                 // includeTags
    NULL,                 // excludeTags
    NULL,                 // indexTags
    NULL,                 // metadataTags
    NULL                  // conflations
  },
  {
    "trecweb",            // name
    "html",               // parser
    "tagged",             // iterator
    "<DOC>",              // startDocTag
    "</DOC>\n",             // endDocTag
    "</DOCHDR>",          // endMetadataTag
    NULL,                 // includeTags
    NULL,                 // excludeTags
    html_index_tags,      // indexTags
    html_metadata_tags,   // metadataTags
    html_conflations      // conflations
  },
  
  {
    "trectext",           // name
    "xml",                // parser
    "tagged",             // iterator
    "<DOC>",              // startDocTag
    "</DOC>\n",             // endDocTag
    NULL,                 // endMetadataTag
    trec_include_tags,    // includeTags
    NULL,                 // excludeTags
    trec_index_tags,      // indexTags
    trec_metadata_tags,   // metadataTags
    trec_conflations      // conflations
  },

#ifdef WIN32
  {
    "doc",                // name
    "text",               // parser
    "doc",                // iterator
    NULL,                 // startDocTag
    NULL,                 // endDocTag
    NULL,                 // endMetadataTag
    NULL,                 // includeTags
    NULL,                 // excludeTags
    NULL,                 // indexTags
    NULL,                 // metadataTags
    NULL                  // conflations
  },

  {
    "ppt",                // name
    "text",               // parser
    "ppt",                // iterator
    NULL,                 // startDocTag
    NULL,                 // endDocTag
    NULL,                 // endMetadataTag
    NULL,                 // includeTags
    NULL,                 // excludeTags
    NULL,                 // indexTags
    NULL,                 // metadataTags
    NULL                  // conflations
  },
#endif

  {
    "pdf",                // name
    "text",               // parser
    "pdf",                // iterator
    NULL,                 // startDocTag
    NULL,                 // endDocTag
    NULL,                 // endMetadataTag
    NULL,                 // includeTags
    NULL,                 // excludeTags
    NULL,                 // indexTags
    NULL,                 // metadataTags
    NULL                  // conflations
  },

  {
    "txt",                // name
    "text",               // parser
    "text",               // iterator
    NULL,                 // startDocTag
    NULL,                 // endDocTag
    NULL,                 // endMetadataTag
    NULL,                 // includeTags
    NULL,                 // excludeTags
    NULL,                 // indexTags
    NULL,                 // metadataTags
    NULL                  // conflations
  },

  // null ending marker
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

static void copy_strings_to_vector( std::vector<std::string>& vec, const char** array ) {
  if( array ) {
    for( unsigned int i=0; array[i]; i++ ) {
      vec.push_back(array[i]);
    }
  }
}

static void copy_string_pairs_to_map( std::map< std::string, std::string >& m, const char** array ) {
  if( array ) {
    for( unsigned int i=0; array[i] && array[i+1]; i+=2 ) {
      std::string key = array[i];
      std::string value = array[i+1];
      m[key] = value;
    }
  }
}

indri::parse::FileClassEnvironmentFactory::~FileClassEnvironmentFactory() { 
  std::map<std::string, indri::parse::FileClassEnvironmentFactory::Specification*>::iterator iter;

  for( iter = _userTable.begin(); iter != _userTable.end(); iter++ ) {
    delete iter->second;
  }
}

indri::parse::FileClassEnvironment* build_file_class_environment( const file_class_environment_spec* spec ) {
  indri::parse::FileClassEnvironment* env = new indri::parse::FileClassEnvironment;
  env->iterator = indri::parse::DocumentIteratorFactory::get( spec->iterator, spec->startDocTag, spec->endDocTag, spec->endMetadataTag );

  std::vector<std::string> includeTags;
  std::vector<std::string> excludeTags;
  std::vector<std::string> indexTags;
  std::vector<std::string> metadataTags;
  std::map<std::string, std::string> conflations;

  copy_strings_to_vector( includeTags, spec->includeTags );
  copy_strings_to_vector( excludeTags, spec->excludeTags );
  copy_strings_to_vector( indexTags, spec->indexTags );
  copy_strings_to_vector( metadataTags, spec->metadataTags );
  copy_string_pairs_to_map( conflations, spec->conflations );

  env->parser = indri::parse::ParserFactory::get( spec->parser, includeTags, excludeTags, indexTags, metadataTags, conflations );
  return env;
}

indri::parse::FileClassEnvironment* build_file_class_environment( const indri::parse::FileClassEnvironmentFactory::Specification * spec ) {
  indri::parse::FileClassEnvironment* env = new indri::parse::FileClassEnvironment;
  env->iterator = indri::parse::DocumentIteratorFactory::get( spec->iterator,
                                                spec->startDocTag.c_str(),
                                                spec->endDocTag.c_str(),
                                                spec->endMetadataTag.c_str() );

  env->parser = indri::parse::ParserFactory::get( spec->parser,
                                    spec->include,
                                    spec->exclude,
                                    spec->index,
                                    spec->metadata,
                                    spec->conflations );

  return env;
}

indri::parse::FileClassEnvironmentFactory::Specification* indri::parse::FileClassEnvironmentFactory::getFileClassSpec( const std::string& name ) {
  // look for a user-specified environment:
  std::map<std::string, indri::parse::FileClassEnvironmentFactory::Specification*>::iterator iter;
  iter = _userTable.find( name );

  if( iter != _userTable.end() ) {
    // copy and return;
    indri::parse::FileClassEnvironmentFactory::Specification* spec = new indri::parse::FileClassEnvironmentFactory::Specification;
    *spec = *(iter->second);
    return spec;
  }
  // look for a default environment
  const file_class_environment_spec* spec = 0;
  for( unsigned int i=0; environments[i].name; i++ ) {
    if( !strcmp( name.c_str(), environments[i].name ) ) {
      spec = &environments[i];
      break;
    }
  }

  if( spec ) {
      indri::parse::FileClassEnvironmentFactory::Specification* newSpec = new indri::parse::FileClassEnvironmentFactory::Specification;


    std::vector<std::string> includeTags;
    std::vector<std::string> excludeTags;
    std::vector<std::string> indexTags;
    std::vector<std::string> metadataTags;
    std::map<std::string, std::string> conflations;

    copy_strings_to_vector( includeTags, spec->includeTags );
    copy_strings_to_vector( excludeTags, spec->excludeTags );
    copy_string_pairs_to_map( conflations, spec->conflations );
    copy_strings_to_vector(indexTags, spec->indexTags);
    copy_strings_to_vector(metadataTags, spec->metadataTags);

    newSpec->name = spec->name;
    newSpec->iterator = spec->iterator;
    newSpec->parser = spec->parser;    
    newSpec->index = indexTags;
    newSpec->metadata = metadataTags;
    newSpec->include = includeTags;
    newSpec->exclude = excludeTags;
    newSpec->conflations = conflations;
    newSpec->startDocTag = spec->startDocTag ? spec->startDocTag : "";
    newSpec->endDocTag = spec->endDocTag ? spec->endDocTag : "" ;
    newSpec->endMetadataTag = spec->endMetadataTag ? spec->endMetadataTag : "";
    return newSpec;
  }
  return 0;
}

indri::parse::FileClassEnvironment* indri::parse::FileClassEnvironmentFactory::get( const std::string& name ) {
  // look for a user-specified environment:
  std::map<std::string, indri::parse::FileClassEnvironmentFactory::Specification*>::iterator iter;
  iter = _userTable.find( name );

  if( iter != _userTable.end() ) {
    return build_file_class_environment( iter->second );
  }

  // look for a default environment
  const file_class_environment_spec* spec = 0;

  for( unsigned int i=0; environments[i].name; i++ ) {
    if( !strcmp( name.c_str(), environments[i].name ) ) {
      spec = &environments[i];
      break;
    }
  }

  if( spec )
    return build_file_class_environment( spec );

  return 0;
}

void indri::parse::FileClassEnvironmentFactory::addFileClass( const std::string& name, 
                                                const std::string& iterator,
                                                const std::string& parser,
                                                const std::string& startDocTag,
                                                const std::string& endDocTag,
                                                const std::string& endMetadataTag,
                                                const std::vector<std::string>& include,
                                                const std::vector<std::string>& exclude,
                                                const std::vector<std::string>&
 index,
                                                const std::vector<std::string>& metadata, 
                                                const std::map<std::string,std::string>& conflations )
{
  indri::parse::FileClassEnvironmentFactory::Specification* spec = new indri::parse::FileClassEnvironmentFactory::Specification;

  spec->name = name;
  spec->iterator = iterator;
  spec->parser = parser;

  spec->include = include;
  spec->exclude = exclude;
  spec->metadata = metadata;
  spec->index = index;
  spec->conflations = conflations;

  spec->startDocTag = startDocTag;
  spec->endDocTag = endDocTag;
  spec->endMetadataTag = endMetadataTag;

  _userTable[spec->name] = spec;
}

void indri::parse::FileClassEnvironmentFactory::addFileClass( const indri::parse::FileClassEnvironmentFactory::Specification &spec) {
  // make a copy
  indri::parse::FileClassEnvironmentFactory::Specification* newSpec = new indri::parse::FileClassEnvironmentFactory::Specification;
  *newSpec = spec;
  // see if there is already an entry for this name
  std::map<std::string, indri::parse::FileClassEnvironmentFactory::Specification*>::iterator iter;
  iter = _userTable.find( newSpec->name );
  // delete it.
  if( iter != _userTable.end() ) {
    delete(iter->second);
  }
  _userTable[newSpec->name] = newSpec;
}
