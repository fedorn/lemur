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
// CompressedCollection.hpp
//
// 12 May 2004 -- tds
//

#ifndef INDRI_COMPRESSEDCOLLECTION_HPP
#define INDRI_COMPRESSEDCOLLECTION_HPP

#include "indri/Collection.hpp"
#include "string-set.h"
#include <string>
#include "File.hpp"
#include "Keyfile.hpp"
#include "WriteBuffer.hpp"
#include "indri/Buffer.hpp"
#include "indri/HashTable.hpp"

class CompressedCollection : public Collection {
private:
  Keyfile _lookup;
  File _storage;
  WriteBuffer* _output;
  Buffer _positionsBuffer;
  struct z_stream_s* _stream;
  HashTable<const char*, Keyfile*> _metalookups;
  String_set* _strings;

  void _writePositions( ParsedDocument* document, int& keyLength, int& valueLength );
  void _writeMetadataItem( ParsedDocument* document, int i, int& keyLength, int& valueLength );
  void _writeText( ParsedDocument* document, int& keyLength, int& valueLength );

  void _readPositions( ParsedDocument* document, const void* positionData, int positionDataLength );

public:
  CompressedCollection();
  ~CompressedCollection();

  void create( const std::string& fileName );
  void create( const std::string& fileName, const std::vector<std::string>& indexedFields );
  void open( const std::string& fileName );
  void openRead( const std::string& fileName );
  void close();

  ParsedDocument* retrieve( int documentID );
  std::string retrieveMetadatum( int documentID, const std::string& attributeName );
  void addDocument( int documentID, ParsedDocument* document );
};

#endif // INDRI_COMPRESSEDCOLLECTION_HPP
