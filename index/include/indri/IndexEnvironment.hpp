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

#ifndef INDRI_INDEXENVIRONMENT_HPP
#define INDRI_INDEXENVIRONMENT_HPP

#include <string>
#include "indri/Parameters.hpp"
#include "indri/HTMLParser.hpp"
#include "indri/Repository.hpp"
#include "indri/IndriParser.hpp"
#include "indri/DocumentIterator.hpp"
#include "indri/AnchorTextAnnotator.hpp"
#include "indri/DocumentIteratorFactory.hpp"
#include "indri/ParserFactory.hpp"
#include "indri/FileClassEnvironmentFactory.hpp"
#include <map>

struct IndexStatus {
  enum action_code {
    FileOpen,
    FileSkip,
    FileError,
    FileClose,
    DocumentCount
  };

  virtual void operator () ( int code, const std::string& documentPath, const std::string& error, int documentsIndexed, int documentsSeen ) {
    status( code, documentPath, error, documentsIndexed, documentsSeen );
  }

  virtual void status( int code, const std::string& documentPath, const std::string& error, int documentsIndexed, int documentsSeen ) {};
};

/*! Principal class for interacting with Indri indexes during index 
  construction. Provides the API for opening or creating an index and its
  associated repository, setting indexing and text parsing parameters, and
  adding documents to the repository.
*/
class IndexEnvironment {
private:
  IndexStatus* _callback;
  Parameters* _options;

  std::string _repositoryPath;
  Repository _repository;
  int _documents;
  std::string _error;

  std::string _anchorTextRoot;
  std::string _documentRoot;

  Parameters _parameters;
  FileClassEnvironmentFactory _fileClassFactory;

  AnchorTextAnnotator _annotator;
  std::map<std::string, FileClassEnvironment*> _environments;

  int _documentsIndexed;
  int _documentsSeen;

  void _getParsingContext( indri::Parser** parser,
                           DocumentIterator** iterator,
                           const std::string& extension );

public:
  IndexEnvironment();
  ~IndexEnvironment();
  /// Set document root path and anchor text root path.
  /// @param documentRoot path to document root.
  /// @param anchorTextRoot path to anchor text root.
  void setAnchorTextPath( const std::string& documentRoot, const std::string& anchorTextRoot );
  /// Add parsing information for a file class. Data for these parameters
  /// is passed into the FileClassEnvironmentFactory
  /// @param name name of this file class, eg trecweb
  /// @param iterator document iterator for this file class
  /// @param parser document tokenizer for this file class
  /// @param startDocTag tag indicating start of a document
  /// @param endDocTag tag indicating the end of a document
  /// @param endMetadataTag tag indicating the end of the metadata fields
  /// @param include default tags whose contents should be included in the index
  /// @param exclude tags whose contents should be excluded from the index
  /// @param index tags that should be forwarded to the index for tag extents
  /// @param metadata tags whose contents should be indexed as metadata
  /// @param conflations tags that should be conflated
  void addFileClass( const std::string& name, 
                     const std::string& iterator,
                     const std::string& parser,
                     const std::string& startDocTag,
                     const std::string& endDogTag,
                     const std::string& endMetadataTag,
                     const std::vector<std::string>& include,
                     const std::vector<std::string>& exclude,
                     const std::vector<std::string>& index,
                     const std::vector<std::string>& metadata, 
                     const std::map<std::string,std::string>& conflations );
  /// getthe named file class.
  FileClassEnvironmentFactory::Specification *getFileClassSpec( const std::string& name) {
    return _fileClassFactory.getFileClassSpec(name);
  }
  /// add a file class.
  void addFileClass( const FileClassEnvironmentFactory::Specification &spec){
    _fileClassFactory.addFileClass(spec);
  }
  

  /// set names of fields to be indexed as data
  /// @param fieldNames the list of fields.
  void setIndexedFields( const std::vector<std::string>& fieldNames );
  void setNumericField( const std::string& fieldName, bool isNumeric );
  /// set names of fields to be indexed as metadata
  /// @param fieldNames the list of fields.
  void setMetadataIndexedFields( const std::vector<std::string>& fieldNames );
  /// set the list of stopwords
  /// @param stopwords the list of stopwords
  void setStopwords( const std::vector<std::string>& stopwords );
  /// set the stemmer to use
  /// @param stemmer the stemmer to use. One of krovetz, porter
  void setStemmer( const std::string& stemmer );
  /// set the amount of memory to use for internal structures
  /// @param memory the number of bytes to use.
  void setMemory( UINT64 memory );
  /// create a new index and repository
  /// @param repositoryPath the path to the repository
  /// @param callback IndexStatus object to be notified of indexing progress.
  void create( const std::string& repositoryPath, IndexStatus* callback = 0 );
  /// open an existing index and repository
  /// @param repositoryPath the path to the repository
  /// @param callback IndexStatus object to be notified of indexing progress.
  void open( const std::string& repositoryPath, IndexStatus* callback = 0 );
  /// close the index and repository
  void close();
  /// add a file to the index and repository
  /// @param fileName the file to add
  void addFile( const std::string& fileName );
  /// add a file of the specified file class to the index and repository
  /// @param fileName the file to add
  /// @param fileClass the file class to add (eg trecweb).
  void addFile( const std::string& fileName, const std::string& fileClass );
  /// add a string to the index and repository
  /// @param documentString the document to add
  /// @param fileClass the file class to add (eg trecweb).
  /// @param metadata the metadata pairs associated with the string.
  void addString( const std::string& documentString, const std::string& fileClass, const std::vector<MetadataPair>& metadata );
  /// add an already parsed document to the index and repository
  /// @param document the document to add
  void addParsedDocument( ParsedDocument* document );
};

#endif // INDRI_INDEXENVIRONMENT_HPP

