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
#include <vector>
#include "Keyfile.hpp"
#include "indri/Buffer.hpp"
#include "indri/SequentialWriteBuffer.hpp"
#include "indri/HashTable.hpp"
#include "indri/File.hpp"
#include "indri/Mutex.hpp"
// hack around to avoid including zlib.h and
// get the struct type into the right namespace.
typedef struct z_stream_s* z_stream_p;

namespace indri
{
  namespace collection
  {
    
    class CompressedCollection : public Collection {
    private:
      indri::thread::Mutex _lock;

      lemur::file::Keyfile _lookup;
      indri::file::File _storage;
      indri::file::SequentialWriteBuffer* _output;
      indri::utility::Buffer _positionsBuffer;
      //  struct z_stream_s* _stream;
      z_stream_p _stream;
      indri::utility::HashTable<const char*, lemur::file::Keyfile*> _reverseLookups;
      indri::utility::HashTable<const char*, lemur::file::Keyfile*> _forwardLookups;
      String_set* _strings;

      void _writePositions( indri::api::ParsedDocument* document, int& keyLength, int& valueLength );
      void _writeMetadataItem( indri::api::ParsedDocument* document, int i, int& keyLength, int& valueLength );
      void _writeText( indri::api::ParsedDocument* document, int& keyLength, int& valueLength );
      void _writeContent( indri::api::ParsedDocument* document, int& keyLength, int& valueLength );
      void _writeContentLength( indri::api::ParsedDocument* document, int& keyLength, int& valueLength );

      void _readPositions( indri::api::ParsedDocument* document, const void* positionData, int positionDataLength );

    public:
      CompressedCollection();
      ~CompressedCollection();

      void create( const std::string& fileName );
      void create( const std::string& fileName, const std::vector<std::string>& indexedFields );
      void create( const std::string& fileName, const std::vector<std::string>& forwardIndexedFields, const std::vector<std::string>& reverseIndexedFields );

      void open( const std::string& fileName );
      void openRead( const std::string& fileName );
      void close();

      indri::api::ParsedDocument* retrieve( int documentID );
      std::string retrieveMetadatum( int documentID, const std::string& attributeName );
      std::vector<indri::api::ParsedDocument*> retrieveByMetadatum( const std::string& attributeName, const std::string& value );
      std::vector<int> retrieveIDByMetadatum( const std::string& attributeName, const std::string& value );

      void addDocument( int documentID, indri::api::ParsedDocument* document );
    };
  }
}

#endif // INDRI_COMPRESSEDCOLLECTION_HPP
