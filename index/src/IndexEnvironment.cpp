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

void IndexEnvironment::_getParsingContext( indri::Parser** parser,
                                           DocumentIterator** iterator,
                                           const std::string& className ) {
  std::string parserName;
  std::string iteratorName;

  *parser = 0;
  *iterator = 0;

  // look for an already-built environment
  std::map<std::string, FileClassEnvironment*>::iterator iter;
  iter = _environments.find(className);

  if( iter != _environments.end() ) {
    *parser = iter->second->parser;
    *iterator = iter->second->iterator;
    return;
  }

  FileClassEnvironment* fce = _fileClassFactory.get( className );

  if( fce ) {
    _environments[className] = fce;
    *parser = fce->parser;
    *iterator = fce->iterator;
  }
}

IndexEnvironment::IndexEnvironment() :
  _callback(0),
  _options(0),
  _documentsIndexed(0),
  _documentsSeen(0)
{
}

IndexEnvironment::~IndexEnvironment() {
  close();
  delete_map_contents<std::string, FileClassEnvironment>( _environments );
}

void IndexEnvironment::setMemory( UINT64 memory ) {
  _parameters.set("memory", memory);
}

void IndexEnvironment::setAnchorTextPath( const std::string& documentRoot, const std::string& anchorTextRoot ) {
  _documentRoot = documentRoot;
  _anchorTextRoot = anchorTextRoot;
}

void IndexEnvironment::setStopwords( const std::vector<std::string>& stopwords ) {
  _parameters.set("stopper","");
  Parameters p = _parameters.get("stopper");
  for( unsigned int i=0; i<stopwords.size(); i++ ) {
    _parameters.append("stopper.word").set(stopwords[i]);
  }
}

void IndexEnvironment::setStemmer( const std::string& stemmer ) {
  _parameters.set("stemmer.name", stemmer);
}

void IndexEnvironment::addFileClass( const std::string& name, 
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
  this->_fileClassFactory.addFileClass( name, iterator, parser, startDocTag, endDocTag, endMetadataTag,
                                              include, exclude, index, metadata, conflations );

}

void IndexEnvironment::setIndexedFields( const std::vector<std::string>& fieldNames ) {
  for( unsigned int i=0; i<fieldNames.size(); i++) { 
    if( i==0 )
      _parameters.set("field.name", fieldNames[i]);
    else
      _parameters.append("field").set("name",fieldNames[i]);
  }
}

void IndexEnvironment::setMetadataIndexedFields( const std::vector<std::string>& fieldNames ) {
  if( !_parameters.exists("collection") )
    _parameters.set("collection", "");

  Parameters collection = _parameters.get("collection");
  for( unsigned int i=0; i<fieldNames.size(); i++ ) {
    if( i==0 )
      collection.set("field", fieldNames[i]);
    else
      collection.append("field").set(fieldNames[i]);
  }
}

void IndexEnvironment::create( const std::string& repositoryPath, IndexStatus* callback ) {
  _callback = callback;
  _repository.create( repositoryPath, &_parameters );
}

void IndexEnvironment::open( const std::string& repositoryPath, IndexStatus* callback ) {
  _callback = callback;
  _repository.open( repositoryPath, &_parameters );
}

void IndexEnvironment::close() {
  _repository.close();
}

void IndexEnvironment::addFile( const std::string& fileName ) {
  std::string extension = Path::extension( fileName );
  addFile( fileName, extension );
}

void IndexEnvironment::addFile( const std::string& fileName, const std::string& fileClass ) {
  indri::Parser* parser = 0;
  DocumentIterator* iterator = 0;
  AnchorTextAnnotator* annotator = 0;
  
  _getParsingContext( &parser, &iterator, fileClass );

  if( !parser || !iterator ) {
    _documentsSeen++;
    if( _callback ) (*_callback) ( IndexStatus::FileSkip, fileName, _error, _documentsIndexed, _documentsSeen );
  } else {
    try {
      UnparsedDocument* document;
      ParsedDocument* parsed;

      iterator->open( fileName );

      if( _anchorTextRoot.length() ) {
        // if the user specified some anchor text, we'll add it in
        std::string relativePath = Path::relative( _documentRoot, fileName );
        std::string anchorTextPath;
        if( relativePath.length() > 0 )
          anchorTextPath = Path::combine( _anchorTextRoot, relativePath );
        else
          anchorTextPath = _anchorTextRoot;
        _annotator.open( anchorTextPath );
        annotator = &_annotator;
      }

      // notify caller that the file was successfully parsed
      if( _callback ) (*_callback)( IndexStatus::FileOpen, fileName, _error, _documentsIndexed, _documentsSeen );

      while( document = iterator->nextDocument() ) {
        _documentsSeen++;

        parsed = parser->parse( document );
        if( annotator )
          parsed = annotator->transform( parsed );
        _repository.addDocument( parsed );

        _documentsIndexed++;
        if( _callback ) (*_callback)( IndexStatus::DocumentCount, fileName, _error, _documentsIndexed, _documentsSeen );
      }

      // notify caller that the file was successfully closed
      if( _callback ) (*_callback)( IndexStatus::FileClose, fileName, _error, _documentsIndexed, _documentsSeen );

      iterator->close();
    } catch( Exception& e ) {
      if( iterator )
        iterator->close();

      // notify caller of errors
      if( _callback ) (*_callback)( IndexStatus::FileError, fileName, e.what(), _documentsIndexed, _documentsSeen );
    }
  }
}

void IndexEnvironment::addString( const std::string& documentString, const std::string& fileClass, const std::vector<MetadataPair>& metadata ) {
  UnparsedDocument document;
  indri::Parser* parser;
  DocumentIterator* iterator;
  std::string nothing;

  _documentsSeen++;

  document.text = documentString.c_str();
  document.textLength = documentString.length();
  document.metadata = metadata;

  _getParsingContext( &parser, &iterator, fileClass );

  ParsedDocument* parsed = parser->parse( &document );
  _repository.addDocument( parsed );

  _documentsIndexed++;
  if( _callback ) (*_callback)( IndexStatus::DocumentCount, nothing, _error, _documentsIndexed, _documentsSeen );
}

void IndexEnvironment::addParsedDocument( ParsedDocument* document ) {
  std::string nothing;

  _documentsSeen++;
  _repository.addDocument( document );
  _documentsIndexed++;
  if( _callback ) (*_callback)( IndexStatus::DocumentCount, nothing, _error, _documentsIndexed, _documentsSeen );
}


