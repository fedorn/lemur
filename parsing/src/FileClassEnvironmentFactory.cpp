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

struct file_class_environment_spec_stl {
  std::string name;
  std::string parser;
  std::string iterator;

  std::string startDocTag;
  std::string endDocTag;
  std::string endMetadataTag;

  std::vector<std::string> include;
  std::vector<std::string> exclude;
  std::vector<std::string> index;
  std::vector<std::string> metadata;
  std::map<std::string,std::string> conflations;
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

FileClassEnvironmentFactory::~FileClassEnvironmentFactory() { 
  std::map<std::string, file_class_environment_spec_stl*>::iterator iter;

  for( iter = _userTable.begin(); iter != _userTable.end(); iter++ ) {
    delete iter->second;
  }
}

FileClassEnvironment* build_file_class_environment( const file_class_environment_spec* spec ) {
  FileClassEnvironment* env = new FileClassEnvironment;
  env->iterator = DocumentIteratorFactory::get( spec->iterator, spec->startDocTag, spec->endDocTag, spec->endMetadataTag );

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

  env->parser = ParserFactory::get( spec->parser, includeTags, excludeTags, indexTags, metadataTags, conflations );
  return env;
}

FileClassEnvironment* build_file_class_environment( const file_class_environment_spec_stl* spec ) {
  FileClassEnvironment* env = new FileClassEnvironment;
  env->iterator = DocumentIteratorFactory::get( spec->iterator,
                                                spec->startDocTag.c_str(),
                                                spec->endDocTag.c_str(),
                                                spec->endMetadataTag.c_str() );

  env->parser = ParserFactory::get( spec->parser,
                                    spec->include,
                                    spec->exclude,
                                    spec->index,
                                    spec->metadata,
                                    spec->conflations );

  return env;
}

FileClassEnvironment* FileClassEnvironmentFactory::get( const std::string& name ) {
  // look for a user-specified environment:
  std::map<std::string, file_class_environment_spec_stl*>::iterator iter;
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

void FileClassEnvironmentFactory::addFileClass( const std::string& name, 
                                                const std::string& iterator,
                                                const std::string& parser,
                                                const std::string& startDocTag,
                                                const std::string& endDocTag,
                                                const std::string& endMetadataTag,
                                                const std::vector<std::string>& include,
                                                const std::vector<std::string>& exclude,
                                                const std::vector<std::string>& index,
                                                const std::vector<std::string>& metadata, 
                                                const std::map<std::string,std::string>& conflations )
{
  file_class_environment_spec_stl* spec = new file_class_environment_spec_stl;

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
