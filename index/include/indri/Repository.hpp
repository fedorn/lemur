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
// Repository
//
// 21 May 2004 -- tds
//

#ifndef INDRI_REPOSITORY_HPP
#define INDRI_REPOSITORY_HPP

#include "indri/Parameters.hpp"
#include "indri/Transformation.hpp"
#include <string>

/*! Encapsulates document manager, index, and field indexes. Provides access 
 *  to collection for both IndexEnvironment and QueryEnvironment.
 */
class Repository {
public:
  struct Field {
    std::string name;
    std::string parserName;
    bool numeric;
  };

private:
  class IndriIndex* _index;
  class CompressedCollection* _collection;
  class TopdocsIndex* _topdocs;
  Parameters _parameters;
  std::vector<Transformation*> _transformations;
  std::vector<Field> _fields;

  std::string _path;
  bool _readOnly;

  INT64 _memory;

  void _buildFields();
  void _buildChain();
  void _buildTransientChain( Parameters& parameters );

  void _copyParameters( Parameters& options );

public:
  Repository() {
    _index = 0;
    _collection = 0;
    _readOnly = false;
  }

  ~Repository() {
    close();
  }
  /// add a parsed document to the repository.
  /// @param document the document to add.
  void addDocument( ParsedDocument* document );
  /// @return the indexed fields for this collection
  const std::vector<Field>& fields() const;
  /// @return the tags for this collection
  std::vector<std::string> tags() const;
  /// Process, possibly transforming, the given term
  /// @param term the term to process
  /// @return the processed term
  std::string processTerm( const std::string& term );
  /// @return the compressed document collection
  class CompressedCollection* collection();
  /// @return the indri document index
  class IndriIndex* index();
  class TopdocsIndex* topdocs();
  /// Create a new empty repository.
  /// @param path the directory to create the repository in
  /// @param options additional parameters
  void create( const std::string& path, Parameters* options = 0 );
  /// Open an existing repository.
  /// @param path the directory to open the repository from
  /// @param options additional parameters
  void open( const std::string& path, Parameters* options = 0 );
  /// Open an existing repository in read only mode.
  /// @param path the directory to open the repository from
  /// @param options additional parameters
  void openRead( const std::string& path, Parameters* options = 0 );
  /// @return true if a valid Indri Repository resides in the named path
  /// false otherwise.
  /// @param path the directory to open the repository from
  static bool exists( const std::string& path );
  /// Close the repository
  void close();
};

#endif // INDRI_REPOSITORY_HPP

