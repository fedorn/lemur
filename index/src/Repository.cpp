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
// Repository
//
// 21 May 2004 -- tds
//

#include "indri/Repository.hpp"
#include "indri/IndriIndex.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/Path.hpp"
#include "indri/PorterStemmerTransformation.hpp"
#include "indri/KrovetzStemmerTransformation.hpp"
#include "indri/StopperTransformation.hpp"
#include "indri/NumericFieldAnnotator.hpp"
#include "indri/Parameters.hpp"
#include "indri/StemmerFactory.hpp"
#include "indri/NormalizationTransformation.hpp"
#include <string>

void Repository::_buildFields() {
  if( _parameters.exists("field") ) {
    Parameters fields = _parameters["field"];

    for( size_t i=0; i<fields.size(); i++ ) {
      Field field;

      field.name = fields[i].get( "name", "" );
      field.numeric = fields[i].get( "numeric", false ) ? true : false;
      field.parserName = fields[i].get( "parserName", "" );

      _fields.push_back(field);
    }
  }
}

void Repository::_buildTransientChain( Parameters& parameters ) {
  if( parameters.exists("stopper.word") ) {
    Parameters stop = parameters["stopper.word"];
    _transformations.push_back( new StopperTransformation( stop ) );
  }
}

void Repository::_buildChain() {
  // TODO: for now, we'll normalize everything, although this should be a parameter
  _transformations.push_back( new NormalizationTransformation() );

  for( size_t i=0; i<_fields.size(); i++ ) {
    if( _fields[i].parserName == "NumericFieldAnnotator" ) {
      _transformations.push_back( new NumericFieldAnnotator( _fields[i].name ) );
    }
  }

  if( _parameters.exists("stopper.word") ) {
    Parameters stop = _parameters["stopper.word"];
    _transformations.push_back( new StopperTransformation( stop ) );
  }

  if( _parameters.exists("stemmer.name") ) {
    std::string stemmerName = std::string(_parameters["stemmer.name"]);
    Parameters stemmerParams = _parameters["stemmer"];
    _transformations.push_back( StemmerFactory::get( stemmerName, stemmerParams ) );
  }
}

std::vector<IndriIndex::FieldDescription> repository_fieldsForIndex( std::vector<Repository::Field>& _fields ) {
  std::vector<IndriIndex::FieldDescription> result;

  for( size_t i=0; i<_fields.size(); i++ ) {
    IndriIndex::FieldDescription fdesc;
    
    fdesc.name = _fields[i].name;
    fdesc.numeric = _fields[i].numeric;

    result.push_back(fdesc);
  }

  return result;
}

void Repository::_copyParameters( Parameters& options ) {
  if( options.exists("field") ) {
    _parameters.set( "field", "" );
    _parameters["field"] = options["field"];
  }

  if( options.exists("stopper") ) {
    _parameters.set( "stopper", "" );
    _parameters["stopper"] = options["stopper"];
  }

  if( options.exists("stemmer") ) {
    _parameters.set( "stemmer", "" );
    _parameters["stemmer"] = options["stemmer"];
  }
}

void Repository::create( const std::string& path, Parameters* options ) {
  _path = path;
  _readOnly = false;

  try {
    if( !Path::exists( path ) ) {
      Path::create( path );
    } else {
      // remove any existing cruft
      Path::remove( path );
      Path::create( path );
    }
    
    int memory = 100*1000*1000;
    if( options )
      memory = options->get( "memory", memory );

    float queryProportion = 0.15f;
    if( options )
      queryProportion = static_cast<float>(options->get( "queryProportion", queryProportion ));
    
    _index = new IndriIndex( memory, queryProportion );
    _collection = new CompressedCollection();

    std::string indexPath = Path::combine( path, "index" );
    std::string collectionPath = Path::combine( path, "collection" );

    if( !Path::exists( indexPath ) )
      Path::create( indexPath );

    std::string indexName = Path::combine( indexPath, "index" );

    if( options )
      _copyParameters( *options );

    _buildFields();
    _buildChain();
    std::vector<IndriIndex::FieldDescription> indexFieldDesc = repository_fieldsForIndex( _fields );

    _index->create( indexName, indexFieldDesc );

    if( !Path::exists( collectionPath ) )
      Path::create( collectionPath );

    std::vector<std::string> collectionFields;

    if( options && options->exists( "collection.field" ) ) {
      Parameters cfields = options->get( "collection.field" );

      for( size_t i=0; i<cfields.size(); i++ ) {
        collectionFields.push_back( (std::string) cfields[i] );
      }
    }

    _collection->create( collectionPath, collectionFields );
  } catch( Exception& e ) {
    LEMUR_RETHROW( e, "Couldn't create a repository at '" + path + "' because:" );
  } catch( ... ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "Something unexpected happened while trying to create '" + path + "'" );
  }
}

void Repository::openRead( const std::string& path, Parameters* options ) {
  _path = path;
  _readOnly = true;

  int memory = 100*1000*1000;
  if( options )
    memory = options->get( "memory", memory );

  float queryProportion = 1;
  if( options )
    queryProportion = static_cast<float>(options->get( "queryProportion", queryProportion ));

  _index = new IndriIndex( memory, queryProportion );
  _collection = new CompressedCollection();

  std::string indexPath = Path::combine( path, "index" );
  std::string collectionPath = Path::combine( path, "collection" );
  std::string indexName = Path::combine( indexPath, "index" );

  _index->openRead( indexName.c_str() );
  _collection->openRead( collectionPath );

  _parameters.loadFile( Path::combine( path, "manifest" ) );

  _buildFields();
  _buildChain();

  if( options )
    _buildTransientChain( *options );
}

void Repository::open( const std::string& path, Parameters* options ) {
  _path = path;
  _readOnly = false;

  int memory = 100*1000*1000;
  if( options )
    memory = options->get( "memory", memory );

  float queryProportion = 0.75;
  if( options )
    queryProportion = static_cast<float>(options->get( "queryProportion", queryProportion ));

  _index = new IndriIndex( memory, queryProportion );
  _collection = new CompressedCollection();

  std::string indexPath = Path::combine( path, "index" );
  std::string collectionPath = Path::combine( path, "collection" );
  std::string indexName = Path::combine( indexPath, "index" );

  _index->open( indexName );
  _collection->open( collectionPath );

  _parameters.loadFile( Path::combine( path, "manifest" ) );

  _buildFields();
  _buildChain();

  if( options )
    _buildTransientChain( *options );
}

bool Repository::exists( const std::string& path ) {
  std::string manifestPath = Path::combine( path, "manifest" );
  return Path::exists( manifestPath );
}

void Repository::addDocument( ParsedDocument* document ) {
  for( size_t i=0; i<_transformations.size(); i++ ) {
    document = _transformations[i]->transform( document );
  }

  int documentID = _index->addDocument( document );
  _collection->addDocument( documentID, document );
}

const std::vector<Repository::Field>& Repository::fields() const {
  return _fields;
}

std::vector<std::string> Repository::tags() const {
  std::vector<std::string> t;

  for( size_t i=0; i<_fields.size(); i++ ) {
    t.push_back(_fields[i].name);
  }

  return t;
}

IndriIndex* Repository::index() {
  return _index;
}

std::string Repository::processTerm( const std::string& term ) {
  ParsedDocument original;
  ParsedDocument* document;
  std::string result;
  char termBuffer[256];
  assert( term.length() < sizeof termBuffer );
  strcpy( termBuffer, term.c_str() );

  original.text = termBuffer;
  original.textLength = strlen(termBuffer)+1;

  original.terms.push_back( termBuffer );
  document = &original;
  
  for( size_t i=0; i<_transformations.size(); i++ ) {
    document = _transformations[i]->transform( document );    
  }
  
  if( document->terms[0] )
    result = document->terms[0];

  return result;
}

CompressedCollection* Repository::collection() {
  return _collection;
}

void Repository::close() {
  if( _index && _collection ) {
    std::string manifest = "manifest";
    std::string paramPath = Path::combine( _path, manifest );

    if( !_readOnly )
      _parameters.writeFile( paramPath );

    _index->close();
    _collection->close();

    delete _index;
    _index = 0;

    delete _collection;
    _collection = 0;

    _fields.clear();
    delete_vector_contents( _transformations );
  }
}


