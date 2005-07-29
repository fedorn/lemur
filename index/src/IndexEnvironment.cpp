/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

//
// IndexEnvironment
//
// 19 July 2004 -- tds
//

#include "indri/IndexEnvironment.hpp"
#include "Exception.hpp"
#include "indri/AnchorTextAnnotator.hpp"
#include "indri/TaggedDocumentIterator.hpp"
#include "indri/IndriParser.hpp"
#include "indri/Path.hpp"

void indri::api::IndexEnvironment::_getParsingContext( indri::parse::Parser** parser,
                                                       indri::parse::DocumentIterator** iterDoc,
                                                       const std::string& className ) {
  std::string parserName;
  std::string iteratorName;

  *parser = 0;
  *iterDoc = 0;

  // look for an already-built environment
  std::map<std::string, indri::parse::FileClassEnvironment*>::iterator iter;
  iter = _environments.find(className);

  if( iter != _environments.end() ) {
    *parser = iter->second->parser;
    *iterDoc = iter->second->iterator;
    return;
  }

  indri::parse::FileClassEnvironment* fce = _fileClassFactory.get( className );

  if( fce ) {
    _environments[className] = fce;
    *parser = fce->parser;
    *iterDoc = fce->iterator;
  }
}

indri::api::IndexEnvironment::IndexEnvironment() :
  _callback(0),
  _options(0),
  _documentsIndexed(0),
  _documentsSeen(0)
{
}

indri::api::IndexEnvironment::~IndexEnvironment() {
  close();
  indri::utility::delete_map_contents<std::string, indri::parse::FileClassEnvironment>( _environments );
}

void indri::api::IndexEnvironment::setNormalization( bool flag ) {
  _parameters.set( "normalize", flag );
}

void indri::api::IndexEnvironment::setMemory( UINT64 memory ) {
  _parameters.set("memory", memory);
}

void indri::api::IndexEnvironment::setAnchorTextPath( const std::string& documentRoot, const std::string& anchorTextRoot ) {
  _documentRoot = documentRoot;
  _anchorTextRoot = anchorTextRoot;
}

void indri::api::IndexEnvironment::setStopwords( const std::vector<std::string>& stopwords ) {
  _parameters.set("stopper","");
  Parameters p = _parameters.get("stopper");
  for( unsigned int i=0; i<stopwords.size(); i++ ) {
    p.append("word").set(stopwords[i]);
  }
}

void indri::api::IndexEnvironment::setStemmer( const std::string& stemmer ) {
  _parameters.set("stemmer.name", stemmer);
}

void indri::api::IndexEnvironment::addFileClass( const std::string& name, 
                                                 const std::string& iter,
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
  this->_fileClassFactory.addFileClass( name, iter, parser, startDocTag, endDocTag, endMetadataTag,
                                        include, exclude, index, metadata, conflations );

}

void indri::api::IndexEnvironment::setIndexedFields( const std::vector<std::string>& fieldNames ) {
  for( unsigned int i=0; i<fieldNames.size(); i++) { 
    if( i==0 )
      _parameters.set("field.name", fieldNames[i]);
    else
      _parameters.append("field").set("name",fieldNames[i]);
  }
}

void indri::api::IndexEnvironment::setNumericField( const std::string& fieldName, bool isNumeric, const std::string & parserName ) {
  if( !_parameters.exists( "field" ) )
    _parameters.set( "field" );

  Parameters fields = _parameters["field"];

  for( int i=0; i<fields.size(); i++ ) {
    std::string parameterFieldName = fields[i]["name"];

    if( parameterFieldName == fieldName ) {
      fields[i].set( "numeric", isNumeric );
      fields[i].set( "parserName", parserName );
      return;
    }
  }
  
  Parameters field = _parameters.append("field");
  field.set( "name", fieldName );
  field.set( "numeric", isNumeric );
  field.set( "parserName", parserName );
}

//
// setMetadataIndexedFields
//

void indri::api::IndexEnvironment::setMetadataIndexedFields( const std::vector<std::string>& forwardFields, const std::vector<std::string>& backwardFields ) {
  if( !_parameters.exists("collection") )
    _parameters.set("collection", "");

  Parameters collection = _parameters.get("collection");
  for( unsigned int i=0; i<forwardFields.size(); i++ ) {
    if( i==0 )
      collection.set("forward", forwardFields[i]);
    else
      collection.append("forward").set(forwardFields[i]);
  }

  for( unsigned int i=0; i<backwardFields.size(); i++ ) {
    if( i==0 )
      collection.set("backward", backwardFields[i]);
    else
      collection.append("backward").set(backwardFields[i]);
  }
}

//
// create
//

void indri::api::IndexEnvironment::create( const std::string& repositoryPath, indri::api::IndexStatus* callback ) {
  _callback = callback;
  _repository.create( repositoryPath, &_parameters );
}

//
// open
//

void indri::api::IndexEnvironment::open( const std::string& repositoryPath, indri::api::IndexStatus* callback ) {
  _callback = callback;
  _repository.open( repositoryPath, &_parameters );
}

//
// close
//

void indri::api::IndexEnvironment::close() {
  _repository.close();
}

//
// addFile
//

void indri::api::IndexEnvironment::addFile( const std::string& fileName ) {
  std::string extension = indri::file::Path::extension( fileName );
  addFile( fileName, extension );
}

//
// addFile
//

void indri::api::IndexEnvironment::addFile( const std::string& fileName, const std::string& fileClass ) {
  indri::parse::Parser* parser = 0;
  indri::parse::DocumentIterator* iterator = 0;
  indri::parse::AnchorTextAnnotator* annotator = 0;
  
  _getParsingContext( &parser, &iterator, fileClass );

  if( !parser || !iterator ) {
    _documentsSeen++;
    if( _callback ) (*_callback) ( indri::api::IndexStatus::FileSkip, fileName, _error, _documentsIndexed, _documentsSeen );
  } else {
    try {
      indri::parse::UnparsedDocument* document;
      ParsedDocument* parsed;

      iterator->open( fileName );

      if( _anchorTextRoot.length() ) {
        // if the user specified some anchor text, we'll add it in
        std::string relativePath = indri::file::Path::relative( _documentRoot, fileName );
        std::string anchorTextPath;
        if( relativePath.length() > 0 )
          anchorTextPath = indri::file::Path::combine( _anchorTextRoot, relativePath );
        else
          anchorTextPath = _anchorTextRoot;
        _annotator.open( anchorTextPath );
        annotator = &_annotator;
      }

      // notify caller that the file was successfully parsed
      if( _callback ) (*_callback)( indri::api::IndexStatus::FileOpen, fileName, _error, _documentsIndexed, _documentsSeen );

      while( document = iterator->nextDocument() ) {
        _documentsSeen++;

        parsed = parser->parse( document );
        if( annotator )
          parsed = annotator->transform( parsed );
        _repository.addDocument( parsed );

        _documentsIndexed++;
        if( _callback ) (*_callback)( indri::api::IndexStatus::DocumentCount, fileName, _error, _documentsIndexed, _documentsSeen );
      }

      // notify caller that the file was successfully closed
      if( _callback ) (*_callback)( indri::api::IndexStatus::FileClose, fileName, _error, _documentsIndexed, _documentsSeen );

      iterator->close();
    } catch( lemur::api::Exception& e ) {
      if( iterator )
        iterator->close();

      // notify caller of errors
      if( _callback ) (*_callback)( indri::api::IndexStatus::FileError, fileName, e.what(), _documentsIndexed, _documentsSeen );
    }
  }
}

//
// addString
//

int indri::api::IndexEnvironment::addString( const std::string& documentString, const std::string& fileClass, const std::vector<indri::parse::MetadataPair>& metadata ) {
  indri::parse::UnparsedDocument document;
  indri::parse::Parser* parser;
  indri::parse::DocumentIterator* iterator;
  std::string nothing;

  _documentsSeen++;

  document.text = documentString.c_str();
  document.textLength = documentString.length();
  document.metadata = metadata;

  _getParsingContext( &parser, &iterator, fileClass );

  if( parser == 0 ) {
    LEMUR_THROW( LEMUR_RUNTIME_ERROR, "File class '" + fileClass + "' wasn't recognized." );
  }

  ParsedDocument* parsed = parser->parse( &document );
  int documentID =_repository.addDocument( parsed );

  _documentsIndexed++;
  if( _callback ) (*_callback)( indri::api::IndexStatus::DocumentCount, nothing, _error, _documentsIndexed, _documentsSeen );

  return documentID;
}

//
// addParsedDocument
//

int indri::api::IndexEnvironment::addParsedDocument( ParsedDocument* document ) {
  std::string nothing;

  _documentsSeen++;
  int documentID = _repository.addDocument( document );
  _documentsIndexed++;
  if( _callback ) (*_callback)( indri::api::IndexStatus::DocumentCount, nothing, _error, _documentsIndexed, _documentsSeen );
  
  return documentID;
}

//
// deleteDocument
//

void indri::api::IndexEnvironment::deleteDocument( int documentID ) {
  _repository.deleteDocument( documentID );
}

//
// documentsIndexed
//

int indri::api::IndexEnvironment::documentsIndexed() {
  return _documentsIndexed;
}

//
// documentsSeen
//

int indri::api::IndexEnvironment::documentsSeen() {
  return _documentsSeen;
}

