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
  Parameters _parameters;
  std::vector<Transformation*> _transformations;
  std::vector<Field> _fields;

  std::string _path;
  bool _readOnly;

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

