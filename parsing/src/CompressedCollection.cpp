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
// CompressedCollection
//
// 12 May 2004 -- tds
//

#include "indri/CompressedCollection.hpp"
#include "zlib.h"
#include "string-set.h"
#include "indri/XMLNode.hpp"
#include "Exception.hpp"
#include "RVLCompress.hpp"
#include "indri/RVLDecompressStream.hpp"
#include "indri/RVLCompressStream.hpp"
#include "indri/Buffer.hpp"
#include "indri/Path.hpp"
#include "indri/Parameters.hpp"
#include "indri/File.hpp"
#include "indri/ScopedLock.hpp"
#include <algorithm>

const int INPUT_BUFFER_SIZE = 1024;
const int OUTPUT_BUFFER_SIZE = 128*1024;
const char POSITIONS_KEY[] = "#POSITIONS#";
const char TEXT_KEY[] = "#TEXT#";
const char CONTENT_KEY[] = "#CONTENT#";
const char CONTENTLENGTH_KEY[] = "#CONTENTLENGTH#";

//
// zlib_alloc
//

static void* zlib_alloc( void* opaque, uInt items, uInt size ) {
  return malloc( items*size );
}

//
// zlib_free
//

static void zlib_free( void* opaque, void* address ) {
  free( address );
}

//
// zlib_deflate
//

static void zlib_deflate( z_stream_s& stream, indri::file::SequentialWriteBuffer* outfile ) {
  if( stream.avail_in == 0 ) {
    // nothing to do...
    return;
  }
  
  if( stream.avail_out == 0 ) {
    stream.next_out = (Bytef*) outfile->write( OUTPUT_BUFFER_SIZE );
    stream.avail_out = OUTPUT_BUFFER_SIZE;
  }

  int result = deflate( &stream, 0 );

  // if we're fine, then just return (common case)
  while( result != Z_OK || stream.avail_in != 0 ) {
    // either we need more space, or an error happened
    if( result != Z_OK ) {
      LEMUR_THROW( LEMUR_IO_ERROR, "Tried to add a document to the collection, but zlib returned an error" );
    }

    // get more space
    stream.next_out = (Bytef*) outfile->write( OUTPUT_BUFFER_SIZE );
    stream.avail_out = OUTPUT_BUFFER_SIZE;
  
    result = deflate( &stream, 0 );
  }
}

//
// zlib_deflate_finish
//

static void zlib_deflate_finish( z_stream_s& stream, indri::file::SequentialWriteBuffer* outfile ) {
  while(true) {
    if( stream.avail_out == 0 ) {
      stream.next_out = (Bytef*) outfile->write( OUTPUT_BUFFER_SIZE );
      stream.avail_out = OUTPUT_BUFFER_SIZE;
    }

    int result = deflate( &stream, Z_FINISH );
    
    if( result == Z_STREAM_END )
      break;

    if( result < 0 )
      LEMUR_THROW( result, "Something bad happened while trying to finish compressing a document." );
  }

  outfile->unwrite( stream.avail_out );
  deflateReset( &stream );
}

//
// zlib_read_document
//

static void zlib_read_document( z_stream_s& stream, indri::file::File& infile, UINT64 offset, indri::utility::Buffer& outputBuffer ) {
  // read in data from the file until the stream ends
  // split up the data as necessary
  // decompress positional info

  // read some data
  char inputBuffer[INPUT_BUFFER_SIZE];
  outputBuffer.grow( INPUT_BUFFER_SIZE );
  outputBuffer.write( sizeof(indri::api::ParsedDocument) );

  stream.avail_in = 0;
  stream.avail_out = 0;
  
  while(true) {
    if( !stream.avail_in ) {
      UINT64 readSize = infile.read( inputBuffer, offset, sizeof inputBuffer );
      offset += readSize; 
      
      stream.avail_in = readSize;
      stream.next_in = (Bytef*) inputBuffer;
    }

    stream.avail_out = outputBuffer.size() - outputBuffer.position();
    stream.next_out = (Bytef*) outputBuffer.write( outputBuffer.size() - outputBuffer.position() );

    int result = inflate( &stream, Z_NO_FLUSH );
    outputBuffer.unwrite( stream.avail_out );

    if( result == Z_STREAM_END ) {
      result = inflate( &stream, Z_FINISH );
      
      if( result < 0 )
        LEMUR_THROW( result, "Something bad happened while trying to finish decompressing a document." );

      inflateEnd( &stream );
      break;
    }

    if( result < 0 ) {
      LEMUR_THROW( result, "Something bad happened while trying to decompress a document." );
    }

    if( stream.avail_out == 0 ) {
      outputBuffer.grow();
    }
  }
}

//
// copy_quad
//

static int copy_quad( char* buffer ) {
  unsigned char firstByte = buffer[0];
  unsigned char secondByte = buffer[1];
  unsigned char thirdByte = buffer[2];
  unsigned char fourthByte = buffer[3];

  int result;

  ( (char*) &result )[0] = firstByte;
  ( (char*) &result )[1] = secondByte;
  ( (char*) &result )[2] = thirdByte;
  ( (char*) &result )[3] = fourthByte;

  return result;
}

//
// _writeMetadataItem
//

void indri::collection::CompressedCollection::_writeMetadataItem( indri::api::ParsedDocument* document, int i, int& keyLength, int& valueLength ) {
  keyLength = strlen(document->metadata[i].key) + 1;
  _stream->next_in = (Bytef*) document->metadata[i].key;
  _stream->avail_in = strlen(document->metadata[i].key) + 1;

  zlib_deflate( *_stream, _output );

  valueLength = document->metadata[i].valueLength;
  _stream->next_in = (Bytef*) document->metadata[i].value;
  _stream->avail_in = valueLength;

  zlib_deflate( *_stream, _output );
}

//
// _writePositions
//

void indri::collection::CompressedCollection::_writePositions( indri::api::ParsedDocument* document, int& keyLength, int& valueLength ) {
  _positionsBuffer.clear();
  _positionsBuffer.grow( document->positions.size() * 10 );
  indri::utility::RVLCompressStream compress( _positionsBuffer );
  int last = 0;

  if( !document->positions.size() )
    return;

  keyLength = sizeof POSITIONS_KEY;
  _stream->next_in = (Bytef*) POSITIONS_KEY;
  _stream->avail_in = keyLength;
  zlib_deflate( *_stream, _output );

  for( size_t i=0; i<document->positions.size(); i++ ) {
    indri::parse::TermExtent extent = document->positions[i];

    compress << (extent.begin - last)
             << (extent.end - extent.begin);

    last = extent.end;
  }

  valueLength = compress.dataSize();
  _stream->next_in = (Bytef*) compress.data();
  _stream->avail_in = valueLength;
  zlib_deflate( *_stream, _output );
}

//
// _writeText
//

void indri::collection::CompressedCollection::_writeText( indri::api::ParsedDocument* document, int& keyLength, int& valueLength ) {
  keyLength = sizeof TEXT_KEY;
  _stream->next_in = (Bytef*) TEXT_KEY;
  _stream->avail_in = keyLength;
  zlib_deflate( *_stream, _output );
  valueLength = document->textLength;
  _stream->next_in = (Bytef*) document->text;
  _stream->avail_in = document->textLength;
  zlib_deflate( *_stream, _output );
}

//
// _writeContent
//
void indri::collection::CompressedCollection::_writeContent( indri::api::ParsedDocument* document, int& keyLength, int& valueLength ) {
  // content (content - text)
  keyLength = sizeof CONTENT_KEY;
  _stream->next_in = (Bytef*) CONTENT_KEY;
  _stream->avail_in = keyLength;
  zlib_deflate( *_stream, _output );
  int diff = document->content - document->text;
  valueLength = sizeof diff;
  _stream->next_in = (Bytef*) &diff;
  _stream->avail_in = sizeof diff;
  zlib_deflate( *_stream, _output );
}


void indri::collection::CompressedCollection::_writeContentLength( indri::api::ParsedDocument* document, int& keyLength, int& valueLength ) {
  // contentLength
  keyLength = sizeof CONTENTLENGTH_KEY;
  _stream->next_in = (Bytef*) CONTENTLENGTH_KEY;
  _stream->avail_in = keyLength;
  zlib_deflate( *_stream, _output );
  int diff = document->contentLength;
  valueLength = sizeof diff;
  _stream->next_in = (Bytef*) &diff;
  _stream->avail_in = sizeof diff;
  zlib_deflate( *_stream, _output );
}

//
// _readPositions
//

void indri::collection::CompressedCollection::_readPositions( indri::api::ParsedDocument* document, const void* positionData, int positionDataLength ) {
  indri::utility::RVLDecompressStream decompress( (const char*) positionData, positionDataLength );
  int last = 0;

  while( !decompress.done() ) {
    indri::parse::TermExtent extent;

    decompress >> extent.begin
               >> extent.end;

    extent.begin += last;
    extent.end += extent.begin;
    last = extent.end;

    document->positions.push_back( extent );
  }
}

//
// CompressedCollection
//

indri::collection::CompressedCollection::CompressedCollection() {
  _stream = new z_stream_s;
  _stream->zalloc = zlib_alloc;
  _stream->zfree = zlib_free;
  _stream->next_out = 0;
  _stream->avail_out = 0;

  deflateInit( _stream, Z_BEST_SPEED );

  _strings = string_set_create();
  _output = 0;
}

//
// ~CompressedCollection
//

indri::collection::CompressedCollection::~CompressedCollection() {
  close();

  delete _output;
  deflateEnd( _stream );
  delete _stream;
  string_set_delete( _strings );
}

//
// create
//

void indri::collection::CompressedCollection::create( const std::string& fileName ) {
  std::vector<std::string> empty;
  create( fileName, empty, empty );
}

//
// create
//

void indri::collection::CompressedCollection::create( const std::string& fileName, const std::vector<std::string>& forwardIndexedFields ) {
  std::vector<std::string> empty;
  create( fileName, forwardIndexedFields, empty );
}

//
// create
//

void indri::collection::CompressedCollection::create( const std::string& fileName, const std::vector<std::string>& forwardIndexedFields, const std::vector<std::string>& reverseIndexedFields ) {
  std::string manifestName = indri::file::Path::combine( fileName, "manifest" );
  std::string lookupName = indri::file::Path::combine( fileName, "lookup" );
  std::string storageName = indri::file::Path::combine( fileName, "storage" );

  _storage.create( storageName );
  _lookup.create( lookupName );
  _output = new indri::file::SequentialWriteBuffer( _storage, 1024*1024 );

  indri::api::Parameters manifest;
  indri::api::Parameters forwardParameters = manifest.append( "forward" );

  for( size_t i=0; i<forwardIndexedFields.size(); i++ ) {
    std::stringstream metalookupName;
    metalookupName << "forwardLookup" << i;

    std::string metalookupPath = indri::file::Path::combine( fileName, metalookupName.str() );
    lemur::file::Keyfile* metalookup = new lemur::file::Keyfile;
    metalookup->create( metalookupPath );

    const char* key = string_set_add( forwardIndexedFields[i].c_str(), _strings );
    _forwardLookups.insert( key, metalookup );
    forwardParameters.append("field").set(forwardIndexedFields[i]);
  }

  indri::api::Parameters reverseParameters = manifest.append( "reverse" );

  for( size_t i=0; i<reverseIndexedFields.size(); i++ ) {
    std::stringstream metalookupName;
    metalookupName << "reverseLookup" << i;

    std::string metalookupPath = indri::file::Path::combine( fileName, metalookupName.str() );
    lemur::file::Keyfile* metalookup = new lemur::file::Keyfile;
    metalookup->create( metalookupPath );

    const char* key = string_set_add( reverseIndexedFields[i].c_str(), _strings );
    _reverseLookups.insert( key, metalookup );
    reverseParameters.append("field").set(reverseIndexedFields[i]);
  }

  manifest.writeFile( manifestName );
}

//
// open
//

void indri::collection::CompressedCollection::open( const std::string& fileName ) {
  std::string lookupName = indri::file::Path::combine( fileName, "lookup" );
  std::string storageName = indri::file::Path::combine( fileName, "storage" );
  std::string manifestName = indri::file::Path::combine( fileName, "manifest" );

  indri::api::Parameters manifest;
  manifest.loadFile( manifestName );

  _storage.open( storageName );
  _lookup.open( lookupName );
  _output = new indri::file::SequentialWriteBuffer( _storage, 1024*1024 );

  if( manifest.exists("forward.field") ) {
    indri::api::Parameters forward = manifest["forward.field"];

    for( size_t i=0; i<forward.size(); i++ ) {
      std::stringstream metalookupName;
      metalookupName << "forwardLookup" << i;

      std::string metalookupPath = indri::file::Path::combine( fileName, metalookupName.str() );
      lemur::file::Keyfile* metalookup = new lemur::file::Keyfile;
      metalookup->open( metalookupPath );

      std::string fieldName = forward[i];
      const char* key = string_set_add( fieldName.c_str(), _strings );
      _forwardLookups.insert( key, metalookup );
    }
  }

  indri::api::Parameters reverse = manifest["reverse"];

  if( manifest.exists("reverse.field") ) {
    indri::api::Parameters reverse = manifest["reverse.field"];

    for( size_t i=0; i<reverse.size(); i++ ) {
      std::stringstream metalookupName;
      metalookupName << "reverseLookup" << i;

      std::string metalookupPath = indri::file::Path::combine( fileName, metalookupName.str() );
      lemur::file::Keyfile* metalookup = new lemur::file::Keyfile;
      metalookup->open( metalookupPath );

      std::string fieldName = reverse[i];
      const char* key = string_set_add( fieldName.c_str(), _strings );
      _reverseLookups.insert( key, metalookup );
    }
  }

}

//
// openRead
//

void indri::collection::CompressedCollection::openRead( const std::string& fileName ) {
  std::string lookupName = indri::file::Path::combine( fileName, "lookup" );
  std::string storageName = indri::file::Path::combine( fileName, "storage" );
  std::string manifestName = indri::file::Path::combine( fileName, "manifest" );

  indri::api::Parameters manifest;
  manifest.loadFile( manifestName );

  _storage.openRead( storageName );
  _lookup.openRead( lookupName );

  if( manifest.exists("forward.field") ) {
    indri::api::Parameters forward = manifest["forward.field"];

    for( size_t i=0; i<forward.size(); i++ ) {
      std::stringstream metalookupName;
      metalookupName << "forwardLookup" << i;

      std::string metalookupPath = indri::file::Path::combine( fileName, metalookupName.str() );
      lemur::file::Keyfile* metalookup = new lemur::file::Keyfile;
      metalookup->openRead( metalookupPath );

      std::string fieldName = forward[i];
      const char* key = string_set_add( fieldName.c_str(), _strings );
      _forwardLookups.insert( key, metalookup );
    }
  }

  if( manifest.exists("reverse.field") ) {
    indri::api::Parameters reverse = manifest["reverse.field"];

    for( size_t i=0; i<reverse.size(); i++ ) {
      std::stringstream metalookupName;
      metalookupName << "reverseLookup" << i;

      std::string metalookupPath = indri::file::Path::combine( fileName, metalookupName.str() );
      lemur::file::Keyfile* metalookup = new lemur::file::Keyfile;
      metalookup->openRead( metalookupPath );

      std::string fieldName = reverse[i];
      const char* key = string_set_add( fieldName.c_str(), _strings );
      _reverseLookups.insert( key, metalookup );
    }
  }
}

//
// close
//

void indri::collection::CompressedCollection::close() {
  _lookup.close();
  if( _output ) {
    _output->flush();
    delete _output;
    _output = 0;
  }

  _storage.close();

  indri::utility::HashTable<const char*, lemur::file::Keyfile*>::iterator iter;

  for( iter = _forwardLookups.begin(); iter != _forwardLookups.end(); iter++ ) {
    (*iter->second)->close();
    delete (*iter->second);
  }

  for( iter = _reverseLookups.begin(); iter != _reverseLookups.end(); iter++ ) {
    (*iter->second)->close();
    delete (*iter->second);
  }

  _forwardLookups.clear();
  _reverseLookups.clear();

  string_set_delete( _strings );
  _strings = string_set_create();
}

//
// File format is:
//    key, value
//    key, value
//    key, value...
// key0, value0, key1, value1 ... , numPairs
//

void indri::collection::CompressedCollection::addDocument( int documentID, indri::api::ParsedDocument* document ) {
  indri::thread::ScopedLock l( _lock );
  
  _stream->zalloc = zlib_alloc;
  _stream->zfree = zlib_free;
  _stream->next_out = 0;
  _stream->avail_out = 0;
  UINT64 offset = _output->tell();
  indri::utility::greedy_vector<UINT32> recordOffsets;
  int keyLength;
  int valueLength;
  int recordOffset = 0;

  // compress the document text, metadata, and term positions
  // record the file position of the start point and index it
  // in the metadata indexes for later retrieval

  // first, write the metadata, storing in metalookups as necessary
  for( size_t i=0; i<document->metadata.size(); i++ ) {
    _writeMetadataItem( document, i, keyLength, valueLength );

    lemur::file::Keyfile** metalookup;
    metalookup = _forwardLookups.find( document->metadata[i].key );

    if( metalookup ) {
      (*metalookup)->put( documentID,
                          document->metadata[i].value,
                          document->metadata[i].valueLength );
    }

    metalookup = _reverseLookups.find( document->metadata[i].key );

    // silently discard any value that is too long to be a key.
    if( metalookup && document->metadata[i].valueLength < lemur::file::Keyfile::MAX_KEY_LENGTH ) {
      // there may be more than one reverse lookup here, so we fetch any old ones:
      indri::utility::greedy_vector<int> documentIDs;
      int dataSize = (*metalookup)->getSize( (const char*)document->metadata[i].value );

      if( dataSize >= 0 ) {
        int actual = 0;

        documentIDs.resize( dataSize / sizeof(int) );
        (*metalookup)->get( (const char*)document->metadata[i].value,
                            &documentIDs.front(),
                            actual,
                            dataSize );
        assert( actual == documentIDs.size() * sizeof(int) );
      }

      documentIDs.push_back( documentID );

      (*metalookup)->put( (const char*)document->metadata[i].value,
                          &documentIDs.front(),
                          documentIDs.size() * sizeof(int) );
    }

    recordOffsets.push_back( recordOffset );
    recordOffsets.push_back( recordOffset + keyLength );
    recordOffset += (keyLength + valueLength);
  }

  // then, write the term positions, after compressing them (delta encoding only, perhaps?)
  _writePositions( document, keyLength, valueLength );
  recordOffsets.push_back( recordOffset );
  recordOffsets.push_back( recordOffset + keyLength );
  recordOffset += (keyLength + valueLength);

  // then, write the text out
  _writeText( document, keyLength, valueLength );
  recordOffsets.push_back( recordOffset );
  recordOffsets.push_back( recordOffset + keyLength );
  recordOffset += (keyLength + valueLength);

  // then, write the content offset out
  _writeContent( document, keyLength, valueLength );
  recordOffsets.push_back( recordOffset );
  recordOffsets.push_back( recordOffset + keyLength );
  recordOffset += (keyLength + valueLength);

  // then, write the content length out
  _writeContentLength( document, keyLength, valueLength );
  recordOffsets.push_back( recordOffset );
  recordOffsets.push_back( recordOffset + keyLength );
  recordOffset += (keyLength + valueLength);

  // finally, we have to write out the keys and values
  recordOffsets.push_back( recordOffsets.size()/2 );
  _stream->next_in = (Bytef*) &recordOffsets.front();
  _stream->avail_in = recordOffsets.size() * sizeof(UINT32);
  zlib_deflate_finish( *_stream, _output );

  // store this data under the document ID
  _lookup.put( documentID, &offset, sizeof offset );
}


indri::api::ParsedDocument* indri::collection::CompressedCollection::retrieve( int documentID ) {
  indri::thread::ScopedLock l( _lock );

  UINT64 offset;
  int actual;
  
  if( !_lookup.get( documentID, &offset, actual, sizeof offset ) ) {
    LEMUR_THROW( LEMUR_IO_ERROR, "Unable to find document " + i64_to_string(documentID) + " in the collection." );
  }

  // flush output buffer; make sure all data is on disk
  if( _output )
    _output->flush();

  // decompress the data
  indri::utility::Buffer output;
  z_stream_s stream;
  stream.zalloc = zlib_alloc;
  stream.zfree = zlib_free;

  inflateInit( &stream );

  zlib_read_document( stream, _storage, offset, output );
  int decompressedSize = stream.total_out;

  // initialize the buffer as a ParsedDocument
  indri::api::ParsedDocument* document = (indri::api::ParsedDocument*) output.front();
  new(document) indri::api::ParsedDocument;

  document->text = 0;
  document->textLength = 0;
  document->content = 0;
  document->contentLength = 0;

  // get the number of fields (it's the last byte)
  char* dataStart = output.front() + sizeof(indri::api::ParsedDocument);
  int fieldCount = copy_quad( dataStart + decompressedSize - 4 );
  int endOffset = decompressedSize - 4 - 2*fieldCount*sizeof(UINT32);
  char* arrayStart = dataStart + endOffset;

  const char* positionData = 0;
  int positionDataLength = 0;

  // store metadata
  for( int i=0; i<fieldCount; i++ ) {
    int keyStart = copy_quad( arrayStart + 2*i*sizeof(UINT32) );
    int valueStart = copy_quad( arrayStart + (2*i+1)*sizeof(UINT32) );
    int valueEnd;

    if( i==(fieldCount-1) ) {
      valueEnd = endOffset;
    } else {
      valueEnd = copy_quad( arrayStart + 2*(i+1)*sizeof(UINT32) );
    }

    indri::parse::MetadataPair pair;
    pair.key = dataStart + keyStart;
    pair.value = dataStart + valueStart;
    pair.valueLength = valueEnd - valueStart;

    // extract text
    if( !strcmp( pair.key, TEXT_KEY ) ) {
      document->text = (char*) pair.value;
      document->textLength = pair.valueLength;
    }

    // extract content
    if( !strcmp( pair.key, CONTENT_KEY ) ) {
      document->content = document->text + copy_quad( (char*) pair.value );
    }

    // extract content length
    if( !strcmp( pair.key, CONTENTLENGTH_KEY ) ) {
      document->contentLength = copy_quad( (char *)pair.value );
    }

    if( !strcmp( pair.key, POSITIONS_KEY ) ) {
      positionData = (char*) pair.value;
      positionDataLength = pair.valueLength;
    }

    document->metadata.push_back( pair );
  }

  // decompress positions
  _readPositions( document, positionData, positionDataLength );

  output.detach();
  return document;
}

//
// retrieveMetadatum
//

std::string indri::collection::CompressedCollection::retrieveMetadatum( int documentID, const std::string& attributeName ) {
  indri::thread::ScopedLock l( _lock );

  lemur::file::Keyfile** metalookup = _forwardLookups.find( attributeName.c_str() );
  std::string result;

  if( metalookup ) {
    char* resultBuffer = 0;
    int length = 0;
    bool success = (*metalookup)->get( documentID, &resultBuffer, length );

    if( success ) {
      // assuming result is of a string type
      result.assign( resultBuffer, length-1 );
    }

    delete[] resultBuffer;
  } else {
    l.unlock();
    indri::api::ParsedDocument* document = retrieve( documentID );
    //This returns the first occurence, rather than the last
    // one gets if a forward lookup table is used.
    /*
      indri::utility::greedy_vector<indri::parse::MetadataPair>::iterator iter = std::find_if( document->metadata.begin(),
      document->metadata.end(),
      indri::parse::MetadataPair::key_equal( attributeName.c_str() ) );
    
      if( iter != document->metadata.end() ) {
      result = (char*) iter->value;
      }
    */
    indri::utility::greedy_vector<indri::parse::MetadataPair>::iterator iter;
    for( iter=document->metadata.begin(); iter !=  document->metadata.end();
         iter++ ) 
      if(!strcmp((*iter).key, attributeName.c_str() ) )
        result = (char*) iter->value;
    delete document;
  }

  return result;
}

//
// retrieveIDByMetadatum 
//

std::vector<int> indri::collection::CompressedCollection::retrieveIDByMetadatum( const std::string& attributeName, const std::string& value ) {
  indri::thread::ScopedLock l( _lock );

  // find the lookup associated with this field
  lemur::file::Keyfile** metalookup = _reverseLookups.find( attributeName.c_str() );
  std::vector<int> results;

  // if we have a lookup, find the associated documentIDs for this value
  if( metalookup ) {
    int actual = 0;
    int dataSize = (*metalookup)->getSize( value.c_str() );

    if( dataSize > 0 ) {
      int actual = 0;
      results.resize( dataSize / sizeof(int), 0 );
      (*metalookup)->get( value.c_str(), &results.front(), actual, dataSize );
      assert( actual == results.size() * sizeof(int) );
    }
  }

  return results;
}

//
// retrieveByMetadatum
//

std::vector<indri::api::ParsedDocument*> indri::collection::CompressedCollection::retrieveByMetadatum( const std::string& attributeName, const std::string& value ) {
  std::vector<indri::api::ParsedDocument*> documents;
  std::vector<int> results = retrieveIDByMetadatum( attributeName, value );

  for( int i=0; i<results.size(); i++ ) {
    documents.push_back( retrieve( results[i] ) );
  }

  return documents;
}

