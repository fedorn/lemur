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
#include <algorithm>

const int INPUT_BUFFER_SIZE = 1024;
const int OUTPUT_BUFFER_SIZE = 128*1024;
const char POSITIONS_KEY[] = "#POSITIONS#";
const char TEXT_KEY[] = "#TEXT#";

static void* zlib_alloc( void* opaque, uInt items, uInt size ) {
  return malloc( items*size );
}

static void zlib_free( void* opaque, void* address ) {
  free( address );
}

static void zlib_deflate( z_stream_s& stream, WriteBuffer* outfile ) {
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

static void zlib_deflate_finish( z_stream_s& stream, WriteBuffer* outfile ) {
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

static void zlib_read_document( z_stream_s& stream, File& infile, File::offset_type offset, Buffer& outputBuffer ) {
  // read in data from the file until the stream ends
  // split up the data as necessary
  // decompress positional info
  infile.seekg( offset, std::fstream::beg );

  // read some data
  char inputBuffer[INPUT_BUFFER_SIZE];
  outputBuffer.grow( INPUT_BUFFER_SIZE );
  outputBuffer.write( sizeof(ParsedDocument) );

  stream.avail_in = 0;
  stream.avail_out = 0;
  
  while(true) {
    if( !stream.avail_in ) {
      infile.read( inputBuffer, sizeof inputBuffer );
      File::offset_type readSize = infile.gcount();

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

void CompressedCollection::_writeMetadataItem( ParsedDocument* document, int i, int& keyLength, int& valueLength ) {
  keyLength = strlen(document->metadata[i].key) + 1;
  _stream->next_in = (Bytef*) document->metadata[i].key;
  _stream->avail_in = strlen(document->metadata[i].key) + 1;

  zlib_deflate( *_stream, _output );

  valueLength = document->metadata[i].valueLength;
  _stream->next_in = (Bytef*) document->metadata[i].value;
  _stream->avail_in = valueLength;

  zlib_deflate( *_stream, _output );
}

void CompressedCollection::_writePositions( ParsedDocument* document, int& keyLength, int& valueLength ) {
  _positionsBuffer.clear();
  _positionsBuffer.grow( document->positions.size() * 10 );
  RVLCompressStream compress( _positionsBuffer.write( _positionsBuffer.size() ), _positionsBuffer.size() );
  int last = 0;

  if( !document->positions.size() )
    return;

  keyLength = sizeof POSITIONS_KEY;
  _stream->next_in = (Bytef*) POSITIONS_KEY;
  _stream->avail_in = keyLength;
  zlib_deflate( *_stream, _output );

  for( size_t i=0; i<document->positions.size(); i++ ) {
    TermExtent extent = document->positions[i];

    compress << (extent.begin - last)
             << (extent.end - extent.begin);

    last = extent.end;
  }

  valueLength = compress.dataSize();
  _stream->next_in = (Bytef*) compress.data();
  _stream->avail_in = valueLength;
  zlib_deflate( *_stream, _output );
}

void CompressedCollection::_writeText( ParsedDocument* document, int& keyLength, int& valueLength ) {
  keyLength = sizeof TEXT_KEY;
  _stream->next_in = (Bytef*) TEXT_KEY;
  _stream->avail_in = keyLength;
  zlib_deflate( *_stream, _output );
  
  _stream->next_in = (Bytef*) document->text;
  _stream->avail_in = document->textLength;
  zlib_deflate( *_stream, _output );
}

void CompressedCollection::_readPositions( ParsedDocument* document, const void* positionData, int positionDataLength ) {
  RVLDecompressStream decompress( (const char*) positionData, positionDataLength );
  int last = 0;

  while( !decompress.done() ) {
    TermExtent extent;

    decompress >> extent.begin
               >> extent.end;

    extent.begin += last;
    extent.end += extent.begin;
    last = extent.end;

    document->positions.push_back( extent );
  }
}

CompressedCollection::CompressedCollection() {
  _output = new WriteBuffer( _storage, 1024*1024 );

  _stream = new z_stream_s;
  _stream->zalloc = zlib_alloc;
  _stream->zfree = zlib_free;
  _stream->next_out = 0;
  _stream->avail_out = 0;

  deflateInit( _stream, Z_DEFAULT_COMPRESSION );

  _strings = string_set_create();
}

CompressedCollection::~CompressedCollection() {
  close();

  delete _output;
  deflateEnd( _stream );
  delete _stream;
  string_set_delete( _strings );
}

void CompressedCollection::create( const std::string& fileName ) {
  std::vector<std::string> empty;
  create( fileName, empty );
}

void CompressedCollection::create( const std::string& fileName, const std::vector<std::string>& indexedFields ) {
  std::string manifestName = Path::combine( fileName, "manifest" );
  std::string lookupName = Path::combine( fileName, "lookup" );
  std::string storageName = Path::combine( fileName, "storage" );

  _storage.open( storageName, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc );
  _lookup.create( lookupName );

  Parameters manifest;

  for( size_t i=0; i<indexedFields.size(); i++ ) {
    std::stringstream metalookupName;
    metalookupName << "metalookup" << i;

    std::string metalookupPath = Path::combine( fileName, metalookupName.str() );
    Keyfile* metalookup = new Keyfile;
    metalookup->create( metalookupPath );

    const char* key = string_set_add( indexedFields[i].c_str(), _strings );
    _metalookups.insert( key, metalookup );
    manifest.append("field").set("name", indexedFields[i]);
  }

  manifest.writeFile( manifestName );
}

void CompressedCollection::open( const std::string& fileName ) {
  std::string lookupName = Path::combine( fileName, "lookup" );
  std::string storageName = Path::combine( fileName, "storage" );
  std::string manifestName = Path::combine( fileName, "manifest" );

  Parameters manifest;
  manifest.loadFile( manifestName );

  _storage.open( storageName, std::fstream::out | std::ifstream::in |  std::fstream::binary );
  _lookup.open( lookupName );
  _storage.seekp( 0, std::fstream::end );

  if( manifest.exists("field") ) {
    Parameters fields = manifest["field"];

    for( size_t i=0; i<fields.size(); i++ ) {
      std::stringstream metalookupName;
      metalookupName << "metalookup" << i;

      std::string metalookupPath = Path::combine( fileName, metalookupName.str() );
      Keyfile* metalookup = new Keyfile;
      metalookup->open( metalookupPath );

      std::string fieldName = fields["name"];
      const char* key = string_set_add( fieldName.c_str(), _strings );
      _metalookups.insert( key, metalookup );
    }
  }
}

void CompressedCollection::openRead( const std::string& fileName ) {
  std::string lookupName = Path::combine( fileName, "lookup" );
  std::string storageName = Path::combine( fileName, "storage" );
  std::string manifestName = Path::combine( fileName, "manifest" );

  Parameters manifest;
  manifest.loadFile( manifestName );

  _storage.open( storageName, std::ifstream::in |  std::fstream::binary );
  _lookup.openRead( lookupName );

  if( manifest.exists("field") ) {
    Parameters fields = manifest["field"];

    for( size_t i=0; i<fields.size(); i++ ) {
      std::stringstream metalookupName;
      metalookupName << "metalookup" << i;

      std::string metalookupPath = Path::combine( fileName, metalookupName.str() );
      Keyfile* metalookup = new Keyfile;
      metalookup->openRead( metalookupPath );

      std::string fieldName = fields["name"];
      const char* key = string_set_add( fieldName.c_str(), _strings );
      _metalookups.insert( key, metalookup );
    }
  }
}

void CompressedCollection::close() {
  _lookup.close();
  if( _output )
    _output->flush();
  _storage.close();

  HashTable<const char*, Keyfile*>::iterator iter;

  for( iter = _metalookups.begin(); iter != _metalookups.end(); iter++ ) {
    (*iter->second)->close();
    delete (*iter->second);
  }

  _metalookups.clear();
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

void CompressedCollection::addDocument( int documentID, ParsedDocument* document ) {
  _stream->zalloc = zlib_alloc;
  _stream->zfree = zlib_free;
  _stream->next_out = 0;
  _stream->avail_out = 0;
  File::offset_type offset = _output->tellp();
  greedy_vector<UINT32> recordOffsets;
  int keyLength;
  int valueLength;
  int recordOffset = 0;

  // compress the document text, metadata, and term positions
  // record the file position of the start point and index it
  // in the metadata indexes for later retrieval

  // first, write the metadata, storing in metalookups as necessary
  for( size_t i=0; i<document->metadata.size(); i++ ) {
    _writeMetadataItem( document, i, keyLength, valueLength );

    Keyfile** metalookup = _metalookups.find( document->metadata[i].key );

    if( metalookup ) {
      (*metalookup)->put( documentID,
                          document->metadata[i].value,
                          document->metadata[i].valueLength );
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

  // finally, we have to write out the keys and values
  recordOffsets.push_back( recordOffsets.size()/2 );
  _stream->next_in = (Bytef*) &recordOffsets.front();
  _stream->avail_in = recordOffsets.size() * sizeof(UINT32);
  zlib_deflate_finish( *_stream, _output );

  // store this data under the document ID
  _lookup.put( documentID, &offset, sizeof offset );
}


ParsedDocument* CompressedCollection::retrieve( int documentID ) {
  File::offset_type offset;
  int actual;
  
  if( !_lookup.get( documentID, &offset, actual, sizeof offset ) ) {
    LEMUR_THROW( LEMUR_IO_ERROR, "Unable to find document " + i64_to_string(documentID) + " in the collection." );
  }

  // decompress the data
  Buffer output;
  z_stream_s stream;
  stream.zalloc = zlib_alloc;
  stream.zfree = zlib_free;

  inflateInit( &stream );

  zlib_read_document( stream, _storage, offset, output );
  int decompressedSize = stream.total_out;

  // initialize the buffer as a ParsedDocument
  ParsedDocument* document = (ParsedDocument*) output.front();
  new(document) ParsedDocument;

  document->text = 0;
  document->textLength = 0;

  // get the number of fields (it's the last byte)
  char* dataStart = output.front() + sizeof(ParsedDocument);
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

    MetadataPair pair;
    pair.key = dataStart + keyStart;
    pair.value = dataStart + valueStart;
    pair.valueLength = valueEnd - valueStart;

    // extract text
    if( !strcmp( pair.key, TEXT_KEY ) ) {
      document->text = (char*) pair.value;
      document->textLength = pair.valueLength;
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

std::string CompressedCollection::retrieveMetadatum( int documentID, const std::string& attributeName ) {
  Keyfile** metalookup = _metalookups.find( attributeName.c_str() );
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
    ParsedDocument* document = retrieve( documentID );

    greedy_vector<MetadataPair>::iterator iter = std::find_if( document->metadata.begin(),
                                                              document->metadata.end(),
                                                              MetadataPair::key_equal( attributeName.c_str() ) );
    
    if( iter != document->metadata.end() ) {
      result = (char*) iter->value;
    }

    delete document;
  }

  return result;
}


