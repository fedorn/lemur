/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.lemurproject.org/license.html 

 *==========================================================================
*/

/*
 * NAME DATE - COMMENTS
 * tnt 01/2003 - created
 * dmf 06/2003 - modify for incremental
 * tds 09/2003 - modified for keyfile
 *========================================================================*/

#include "indri/IndriIndex.hpp"

#include "indri/DocListDiskBuilder.hpp"
#include "indri/DocListMemoryBuilder.hpp"

#include "ReadBuffer.hpp"
#include "WriteBuffer.hpp"

#include "RVLCompress.hpp"

#include "indri/IndriTermInfoList.hpp"

#include <assert.h>
#include <functional>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

#include <time.h>
#include <indri/greedy_vector>
#include <indri/count_iterator>
#include "indri/XMLNode.hpp"
#include "indri/XMLWriter.hpp"
#include "indri/XMLReader.hpp"
#include "indri/delete_range.hpp"

#include "indri/RVLCompressStream.hpp"
#include "indri/RVLDecompressStream.hpp"

#include "indri/ParsedDocument.hpp"
#include <math.h>
#include "indri/Path.hpp"

// suffixes for filenames
#define TERMDATA ".termdata"

#define DOCINDEX  ".docs"
#define DOCSTATS ".docstats"
#define DOCLENGTHS ".doclengths"
#define DOCSTATSBATCH ".docstatsbatch"
#define DOCINDEXBATCH ".docsbatch"

#define TERMIDMAP  ".tid"
#define TERMIDSTRMAP ".tidstr"
#define DOCIDMAP  ".did"
#define DOCIDSTRMAP ".didstr"
#define IVLINDEX  ".ivl"

// what to call out of vocabulary ids
#define INVALID_STR "[OOV]"

#define EXTENSION ".ind"

#define INDRI_MAX_BATCH_MERGE_SEGMENTS                (384)
#define INDRI_MERGE_READBUFFER_SIZE             (1024*1024)
#define INDRI_MERGE_TERMLISTBUFFER_SIZE         (1024*1024)

// 5mb for background memory usage
#define INDRI_BASE_MEMORY_USAGE               (5*1024*1024)
#define INDRI_WRITEBUFFER_SIZE                  (1024*1024)
#define INDRI_DOCLISTREADER_SIZE                (1024*1024)

#define INDRI_MINIMUM_LOOKUP_BUFFER_SIZE         (128*1024)
#define INDRI_MINIMUM_SCRATCH_SPACE              (512*1024)
#define INDRI_EPSILON_FLUSH_POSTINGS             (512*1024)

#define INDRI_FIELD_BUFFERSIZE                  (1024*1024)

#define INDRI_TERMDATA_PROPORTION                    (0.05)
#define INDRI_TERMCACHE_PROPORTION                   (0.10)
#define INDRI_LISTS_PROPORTION                       (0.85)

std::string IndriIndex::_buildFileName( const char* suffix, int index ) {
  std::stringstream s;
  s << _baseName << suffix << index;
  return s.str();
}

std::string IndriIndex::_buildFileName( const char* suffix ) {
  std::stringstream s;
  s << _baseName << suffix;
  return s.str();
}

IndriIndex::IndriIndex( size_t memorySize, float queryProportion ) {
  _computeMemoryBounds( memorySize, queryProportion );
  _termDataTable = new HashTable<int, indri::index::TermData*>( _termDataSize );
  _cache = new HashTable<const char*, term_cache_entry*>( _termCacheSize );
  _writingDocTermLists = true;

  _documentStatisticsBuffer = 0;
  _documentLengthBuffer = 0;
  _documentTermLocationsFile = 0;
  _documentStatisticsBuffer = 0;
  _documentStatisticsFile = 0;

  _batchBuild = false;
  _readOnly = true;
  _batchSegmentCount = 0;
}

IndriIndex::~IndriIndex() {
  close();
}

void IndriIndex::_resetEstimatePoint() {
  // how many postings should we queue up before
  // trying to estimate a flush time?
  // pick the worst case--one unique term per document
  // that would give us one indri::index::DocListMemoryBuilder plus a few bytes
  // for the list buffer:

  _estimatePoint = _corpusStatistics.totalTerms + _listsSize / sizeof(indri::index::TermData);
  _lastCacheFlush = _corpusStatistics.totalTerms;
}

void IndriIndex::_computeMemoryBounds( size_t memorySize, float queryProportion ) {
  _memorySize = lemur_compat::max<size_t>( memorySize, INDRI_BASE_MEMORY_USAGE +
                                                       INDRI_MINIMUM_SCRATCH_SPACE +
                                                       INDRI_MINIMUM_LOOKUP_BUFFER_SIZE );

  size_t space = _memorySize - INDRI_BASE_MEMORY_USAGE;
  _queryProportion = queryProportion;

  _statisticsBufferSize = lemur_compat::max<size_t>( INDRI_MINIMUM_LOOKUP_BUFFER_SIZE,
                                                     (size_t) (_queryProportion * space * 0.15) );
  space -= lemur_compat::min<size_t>(_statisticsBufferSize, space );
  _lengthBufferSize = lemur_compat::max<size_t>( INDRI_MINIMUM_LOOKUP_BUFFER_SIZE,
                                                 (size_t) (_queryProportion * space * 0.85) );
  space -= lemur_compat::min<size_t>(_lengthBufferSize, space );

  space = lemur_compat::max<size_t>(space, INDRI_MINIMUM_SCRATCH_SPACE);
  _listsSize = size_t(INDRI_LISTS_PROPORTION * space);
  _termDataSize = size_t(INDRI_TERMDATA_PROPORTION * space);
  _termCacheSize = size_t(INDRI_TERMCACHE_PROPORTION * space);

  _resetEstimatePoint();
}

void IndriIndex::_openSegments() {
  // open existing inverted list segments
  for( int i=0; i<INDRI_MAX_SEGMENTS; i++ ) {
    File* in = new File();
    std::string name = _buildFileName( IVLINDEX, i );
    in->open( name.c_str(), std::fstream::in | std::fstream::binary );

    if( in->rdstate() & std::fstream::failbit ) {
      delete in;
      break;
    } else {
      _segments.push_back( in );
    }
  }
}

void IndriIndex::_openReadOnlyDBs() {
  std::string docStatsName = _buildFileName( DOCSTATS );

  _documentStatisticsFile = new File();
  _documentStatisticsFile->open( docStatsName, std::ios::binary | std::ios::in );
  _documentStatisticsFile->seekg( 0, std::ios::beg );
  _documentStatisticsBuffer = new ReadBuffer( *_documentStatisticsFile, _statisticsBufferSize );

  std::string docLengthName = _buildFileName( DOCLENGTHS );

  _documentLengthFile.open( docLengthName, std::ios::binary | std::ios::in );
  UINT64 docLengthFileLength = _documentLengthFile.size();
  _documentLengthFile.seekg( 0, std::ios::beg );
  _documentLengthBuffer = new ReadBuffer( _documentLengthFile, std::min<size_t>(_lengthBufferSize, docLengthFileLength) );

  std::string termDataName = _buildFileName( TERMDATA );
  _termDataStore.openRead( termDataName );

  _documentMap.openRead( _baseName, DOCIDMAP, DOCIDSTRMAP );
  _termMap.openRead( _baseName, TERMIDMAP, TERMIDSTRMAP );
}

void IndriIndex::_openDBs() {
  std::string docStatsName = _buildFileName( DOCSTATS );

  _documentStatisticsFile = new File();
  _documentStatisticsFile->open( docStatsName, std::ios::binary | std::ios::in | std::ios::out );
  _documentStatisticsFile->seekg( 0, std::ios::beg );
  _documentStatisticsFile->seekp( 0, std::ios::end );
  _documentStatisticsBuffer = new ReadBuffer( *_documentStatisticsFile, _statisticsBufferSize );

  std::string docLengthName = _buildFileName( DOCLENGTHS );

  _documentLengthFile.open( docLengthName, std::ios::binary | std::ios::in | std::ios::out );
  UINT64 docLengthFileLength = _documentLengthFile.size();
  _documentLengthFile.seekg( 0, std::ios::beg );
  _documentLengthFile.seekp( 0, std::ios::end );
  _documentLengthBuffer = new ReadBuffer( _documentLengthFile, std::min<size_t>(_lengthBufferSize, docLengthFileLength) );

  std::string termDataName = _buildFileName( TERMDATA );
  _termDataStore.open( termDataName );

  _documentMap.open( _baseName, DOCIDMAP, DOCIDSTRMAP );
  _termMap.open( _baseName, TERMIDMAP, TERMIDSTRMAP );
}

void IndriIndex::_createDBs() {
  std::string docLengthName = _buildFileName( DOCLENGTHS );

  _documentLengthFile.open( docLengthName, std::ios::binary | std::ios::out );
  _documentLengthBuffer = new ReadBuffer( _documentLengthFile, _lengthBufferSize );

  std::string termdataName = _buildFileName( TERMDATA );
  _termDataStore.create( termdataName );

  _documentMap.create( _baseName, DOCIDMAP, DOCIDSTRMAP );
  _termMap.create( _baseName, TERMIDMAP, TERMIDSTRMAP );
}

void IndriIndex::_createFields( const std::vector<FieldDescription>& fields ) {
  for( unsigned int i=0; i<fields.size(); i++ ) {
    indri::index::FieldData* fieldData = new indri::index::FieldData( fields[i].name, fields[i].numeric );
    fieldData->create( _buildFileName( ".field", i+1 ) );
    _fieldData.push_back( fieldData );
    _fieldLookup.insert( std::make_pair( fieldData->statistics.name.c_str(), i+1 ) ); 
  }
}

void IndriIndex::_closeFields() {
  for( unsigned int i=0; i<_fieldData.size(); i++ ) {
    _fieldData[i]->close();
    delete _fieldData[i];
  }
}

bool IndriIndex::create( const std::string& indexName, const std::vector<FieldDescription>& fields ) {
  _baseName = indexName;
  _batchBuild = true;
  _readOnly = false;

  _documentTermLocationsFile = 0;
  _documentStatisticsFile = 0;

  _createDBs();
  _createFields( fields );

  // add reserved OOV term
  _termMap.put( 0, INVALID_STR );
  _documentMap.put( 0, INVALID_STR );
  _resetEstimatePoint();

  return true;
}

bool IndriIndex::open( const char* indexName ) {
  std::string name = indexName;
  return open( name );
}

bool IndriIndex::open(const std::string& indexName){
  _batchBuild = false;
  _readOnly = false;

  std::string prefix = Path::basename( indexName );
  setName(prefix);

  std::string parametersName = _baseName + EXTENSION;
  if( ! _readParameters( parametersName ) )
    return false;

  _openDBs();
  _openSegments();

  std::string docfname = _buildFileName( DOCINDEX );
  _documentTermLocationsFile = new File();
  _documentTermLocationsFile->open( docfname.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::ate );
  _documentTermLocationsFile->seekg( 0, std::ios::beg );

  _resetEstimatePoint();
  return true;
}

bool IndriIndex::openRead(const std::string& indexName) {
  _baseName = indexName;
  _batchBuild = false;
  _readOnly = true;

  std::string prefix = Path::basename( indexName );
  setName(prefix);

  std::string parametersName = _baseName + EXTENSION;
  if( ! _readParameters( parametersName ) )
    return false;

  _openReadOnlyDBs();
  _openSegments();

  std::string docfname = _buildFileName( DOCINDEX );
  _documentTermLocationsFile = new File();
  _documentTermLocationsFile->open( docfname.c_str(), std::ios::in | std::ios::binary );
  _documentTermLocationsFile->seekg( 0, std::ios::beg );

  _resetEstimatePoint();
  return true;
}

void IndriIndex::close() {
  if( !_cache )
    return; // index is already closed
  
  if( ! _readOnly ) {
    _writeAndMerge();
    _writeParameters( _baseName + EXTENSION );
  }

  for( unsigned int i=0; i<_segments.size(); i++ ) {
    _segments[i]->close();
    delete _segments[i];
  }

  _clearTermData();
  delete _termDataTable;
  _termDataTable = 0;

  _clearTermCache();
  delete _cache;
  _cache = 0;

  _termDataStore.close();
  _closeFields();
  
  delete _documentTermLocationsFile;
  _documentTermLocationsFile = 0;
  delete _documentStatisticsFile;
  _documentStatisticsFile = 0;

  _documentMap.close();
  _termMap.close();

  delete _documentStatisticsBuffer;
  _documentStatisticsBuffer = 0;
  _documentLengthFile.close();
  delete _documentLengthBuffer;
}

//
// term(const string &)
//

TERMID_T IndriIndex::term(const TERM_T &word) const{
  assert( !_batchBuild );
  TERMID_T termID = _termMap.lookupID( word.c_str() );

  if( termID > 0 )
    return termID;

  return 0;
}

//
// term(int)
//

const TERM_T IndriIndex::term(TERMID_T termID) const{
  assert( !_batchBuild );
  return _termMap.lookupWord( termID );
}

//
// document(const string &)
//

DOCID_T IndriIndex::document(const EXDOCID_T &docIDStr) const{
  DOCID_T documentID = _documentMap.lookupID( docIDStr.c_str() );

  if( documentID > 0 )
    return documentID;

  return 0;
}

//
// document(int)
//

const EXDOCID_T IndriIndex::document(DOCID_T docID) const {
  return _documentMap.lookupWord( docID );
}

//
// field(const char*)
//

int IndriIndex::field( const char* fieldName ) {
  std::map<const char*,int,less_string>::iterator iter;

  iter = _fieldLookup.find( fieldName );

  if( iter != _fieldLookup.end() )
    return iter->second;

  return 0;
}

//
// field(const std::string&)
//

int IndriIndex::field( const std::string& fieldName ) {
  return field( fieldName.c_str() );
}

//
// field(int)
//

const char* IndriIndex::field( int fieldID ) {
  assert( fieldID >= 0 && fieldID <= _fieldData.size() );

  if( fieldID == 0 )
    return "[OOV]";

  return _fieldData[fieldID-1]->statistics.name.c_str();
}

INT64 IndriIndex::termCount(TERMID_T termID) const {
  if( termID == 0 )
    return 0;

  indri::index::TermData* termData = const_cast<IndriIndex*>(this)->_fetchTermData( termID );
  
  if( termData ) {
    return termData->corpus.totalCount;
  } else {
    return 0;
  }
}

INT64 IndriIndex::fieldTermCount( int fieldID, TERMID_T termID ) const {
  if( unsigned(fieldID) > _fieldData.size() || fieldID <= 0 || _batchBuild )
    return 0;

  if( termID <= 0 || termID > _corpusStatistics.uniqueTerms )
    return 0;

  indri::index::TermData* termData = const_cast<IndriIndex*>(this)->_fetchTermData( termID );

  if( termData ) {
    return termData->fields[fieldID-1].totalCount;
  } else {
    return 0;
  }
}

INT64 IndriIndex::fieldTermCount( int fieldID ) const {
  if( unsigned(fieldID) > _fieldData.size() || fieldID <= 0 || _batchBuild )
    return 0;

  return _fieldData[fieldID-1]->statistics.totalCount;
}

INT64 IndriIndex::fieldDocCount( int fieldID ) const {
  if( unsigned(fieldID) > _fieldData.size() || fieldID <= 0 || _batchBuild )
    return 0;

  return _fieldData[fieldID-1]->statistics.documentCount;
}


INT64 IndriIndex::fieldDocCount( int fieldID, TERMID_T termID ) const {
  if( unsigned(fieldID) > _fieldData.size() || fieldID <= 0 || _batchBuild )
    return 0;

  if( termID <= 0 || termID > _corpusStatistics.uniqueTerms )
    return 0;

  indri::index::TermData* termData = const_cast<IndriIndex*>(this)->_fetchTermData( termID );

  if( termData ) {
    return termData->fields[fieldID-1].documentCount;
  } else {
    return 0;
  }
}

double IndriIndex::docLengthAvg() const {
  if( _corpusStatistics.totalDocuments ) {
    return double(_corpusStatistics.totalTerms) / double(_corpusStatistics.totalDocuments);
  } else {
    return 0;
  }
}

COUNT_T IndriIndex::docCount(TERMID_T termID) const {
  if( termID == 0 || _batchBuild )
    return 0;

  indri::index::TermData* termData = const_cast<IndriIndex*>(this)->_fetchTermData( termID );

  if( termData ) {
    return termData->corpus.documentCount;
  } else {
    return 0;
  }
}

//
// termMaxDocumentFraction
//

double IndriIndex::termMaxDocumentFraction( TERMID_T termID ) {
  if( termID == 0 || _batchBuild )
    return 0;

  indri::index::TermData* termData = const_cast<IndriIndex*>(this)->_fetchTermData( termID );

  if( termData ) {
    return termData->maxDocumentFraction;
  } else {
    return 0;
  }
}

//
// termMaxDocumentFrequency
//

int IndriIndex::termMaxDocumentFrequency( TERMID_T termID ) {
  if( termID == 0 || _batchBuild )
    return 0;

  indri::index::TermData* termData = _fetchTermData( termID );

  if( termData ) {
    return termData->maxDocumentFrequency;
  } else {
    return 0;
  }
}

//
// termMinDocumentLength
//

int IndriIndex::termMinDocumentLength( TERMID_T termID ) {
  if( termID == 0 || _batchBuild )
    return 0;

  indri::index::TermData* termData = _fetchTermData( termID );

  if( termData ) {
    return termData->minDocumentLength;
  } else {
    return 0;
  }
}

//
// maxDocumentLength
//

int IndriIndex::maxDocumentLength() {
  return _corpusStatistics.maximumDocumentLength;
}

//
// documentLength
//

COUNT_T IndriIndex::docLength( DOCID_T documentID ) const {
  return fetchDocumentLength( documentID );
}

//
// documentIndexedLength
//

COUNT_T IndriIndex::docIndexedLength( DOCID_T documentID ) const {
  indri::index::DocumentData data = fetchDocumentData( documentID );
  return data.indexedLength;
}

indri::index::FieldListIterator* IndriIndex::fieldPositionListIterator( int fieldID ) {
  if( fieldID == 0 || _batchBuild )
    return NULL;

  assert( fieldID <= _fieldData.size() );
  return new indri::index::FieldListIterator( *_fieldData[fieldID-1]->listFile, _fieldData[fieldID-1]->statistics.isNumeric );
}

DocPositionInfoList* IndriIndex::docPositionInfoList(TERMID_T termID) {
  if ((termID < 0) || (termID > _corpusStatistics.uniqueTerms) || termID == 0) {
    std::stringstream term; term << termID;
    LEMUR_THROW( LEMUR_IO_ERROR, "Failed to retrieve docPositionInfoList for termID: " + term.str() );
  }

  indri::index::TermData* termData = _fetchTermData( termID );
  assert( termData->segmentOffsets[0] != MAX_UINT64 );

  indri::index::TermFieldStatistics* statistics = &termData->corpus;

  // we have to build the doc info list from the segments on disk
  // TODO: (bug) right now, this method requires that the docInfoLists be
  // all merged up before you can query them--which isn't what we want
  File* segment = _segments[0];
  segment->seekg( 0, std::fstream::beg );

  // give the iterator a general idea of how much data we think we'll be reading
  size_t estimatedDataLength = termData->corpus.totalCount * 2 +
                               termData->corpus.documentCount * 4;

  return new indri::index::DocListIterator( *segment, termID, termData->segmentOffsets[0], estimatedDataLength );
}

DocInfoList* IndriIndex::docInfoList(TERMID_T termID) const {
  return const_cast<IndriIndex*>(this)->docPositionInfoList(termID);
}

indri::index::DocListFrequencyIterator* IndriIndex::docFrequencyInfoList(TERMID_T termID) {
  if ((termID < 0) || (termID > _corpusStatistics.uniqueTerms) || termID == 0) {
    std::stringstream term; term << termID;
    LEMUR_THROW( LEMUR_IO_ERROR, "Failed to retrieve docFrequencyInfoList for termID: " + term.str() );
  }

  indri::index::TermData* termData = _fetchTermData( termID );
  assert( termData->segmentOffsets[0] != MAX_UINT64 );

  indri::index::TermFieldStatistics* statistics = &termData->corpus;

  File* segment = _segments[0];

  // give the iterator a general idea of how much data we think we'll be reading
  size_t estimatedDataLength = termData->corpus.totalCount * 2 +
                               termData->corpus.documentCount * 4;

  return new indri::index::DocListFrequencyIterator( *segment, termID, termData->segmentOffsets[0], estimatedDataLength );
}

TermInfoList* IndriIndex::termInfoList(DOCID_T docID) const {
  indri::index::TermListBuilder* list = const_cast<IndriIndex*>(this)->termPositionList(docID);
  return new indri::index::BagList( list );
}

TermInfoList* IndriIndex::termInfoListSeq(DOCID_T docID) const {
  indri::index::TermListBuilder* list = const_cast<IndriIndex*>(this)->termPositionList(docID);
  return new indri::index::PositionList( list );
}

indri::index::TermListBuilder* IndriIndex::termPositionList( DOCID_T docID ) {
  indri::index::TermListBuilder* result = new indri::index::TermListBuilder;
  indri::index::DocumentData data = fetchDocumentData(docID);
  
  assert( data.byteLength > 0 );
  assert( !_batchBuild );

  Buffer buffer;

  // seek to list position
  _documentTermLocationsFile->seekg( data.offset, std::ios::beg );
  _documentTermLocationsFile->read( buffer.write(data.byteLength), data.byteLength );

  // read the file data
  result->read(buffer.front(), buffer.position());
  return result;
}

/*=======================================================================
 * PRIVATE METHODS 
 =======================================================================*/

void IndriIndex::_writeParameters( const std::string& fileName ) {
  XMLNode* root = new XMLNode( "parameters" );

  // general stuff
  root->addChild( new XMLNode( "index", "IndriIndex" ) );
  root->addChild( new XMLNode( "code-build-date", __DATE__ ) );

  // corpus statistics
  XMLNode* corpus = new XMLNode( "corpus" );

  corpus->addChild( new XMLNode( "total-documents", i64_to_string( _corpusStatistics.totalDocuments ) ) );
  corpus->addChild( new XMLNode( "total-terms", i64_to_string( _corpusStatistics.totalTerms ) ) );
  corpus->addChild( new XMLNode( "unique-terms", i64_to_string( _corpusStatistics.uniqueTerms ) ) );
  corpus->addChild( new XMLNode( "maximum-document-length", i64_to_string( _corpusStatistics.maximumDocumentLength ) ) );

  root->addChild( corpus );

  // field statistics
  XMLNode* fields = new XMLNode( "fields" );

  for( unsigned int i=0; i<_fieldData.size(); i++ ) {
    XMLNode* field = new XMLNode( "field" );

    field->addAttribute( "name", _fieldData[i]->statistics.name );
    field->addAttribute( "isNumeric", _fieldData[i]->statistics.isNumeric ? "true" : "false" );

    field->addChild( new XMLNode( "total-documents", i64_to_string(_fieldData[i]->statistics.documentCount) ) );
    field->addChild( new XMLNode( "total-terms", i64_to_string(_fieldData[i]->statistics.totalCount) ) );

    fields->addChild(field);
  }

  root->addChild(fields);

  // write it all out to disk
  std::string output;
  XMLWriter writer(root);
  writer.write(output);

  std::fstream parameterFile;
  parameterFile.open( fileName.c_str(), std::fstream::out | std::fstream::trunc );
  parameterFile.write( output.c_str(), std::streamsize(output.length()) );
  parameterFile.close();

  delete root;
}

//
// _readParameters
// 

bool IndriIndex::_readParameters( const std::string& fileName ) {
  std::fstream parameterFile;
  parameterFile.open( fileName.c_str(), std::fstream::in );
  if( parameterFile.rdstate() & std::ios::failbit )
    return false;

  // find out how long it is
  parameterFile.seekg( 0, std::fstream::end );
  int length = parameterFile.tellg();
  parameterFile.seekg( 0, std::fstream::beg );
  std::auto_ptr<char> buffer( new char[length] );

  // read it in
  parameterFile.read( buffer.get(), length );
  parameterFile.close();

  XMLNode* node = NULL;

  try {
    XMLReader reader;
    node = reader.read( buffer.get(), length );

    // read the parameters in
    // read corpus stats
    const XMLNode* corpus = node->getChild( "corpus" );

    _corpusStatistics.totalDocuments = string_to_int( corpus->getChild( "total-documents" )->getValue() );
    _corpusStatistics.totalTerms = string_to_i64( corpus->getChild( "total-terms" )->getValue() );
    _corpusStatistics.uniqueTerms = string_to_int( corpus->getChild( "unique-terms" )->getValue() );
    _corpusStatistics.maximumDocumentLength = string_to_int( corpus->getChild( "maximum-document-length" )->getValue() );

    // read the field stats
    const XMLNode* fields = node->getChild( "fields" );
    const std::vector<XMLNode*>* children = &fields->getChildren();

    for( unsigned int i=0;
         i < children->size();
         i++ ) {
      const std::string& fieldName = (*children)[i]->getAttribute( "name" );
      bool isNumeric = ((*children)[i]->getAttribute( "isNumeric" ) == "true");
      INT64 fieldTermCount = string_to_i64( (*children)[i]->getChildValue( "total-terms" ) );
      int fieldDocumentCount = string_to_int( (*children)[i]->getChildValue( "total-documents" ) );

      indri::index::FieldData* fieldData = new indri::index::FieldData( fieldName, isNumeric );
      std::string fieldFileName = _buildFileName( ".field", i+1 );
      if( _readOnly )
        fieldData->openRead( fieldFileName );
      else
        fieldData->open( fieldFileName );
      fieldData->statistics.totalCount = fieldTermCount;
      fieldData->statistics.documentCount = fieldDocumentCount;
      _fieldLookup.insert( std::make_pair( fieldData->statistics.name.c_str(), i+1 ) );
      _fieldData.push_back( fieldData );
    }
  } catch( ... ) {
    if( node ) {
      delete node;
    }

    return false;
  }

  delete node;
  return true;
}

void IndriIndex::setName(const std::string& prefix) {
  _baseName = prefix;
}

// ------------------------------------------------------------------------------
// Insert document methods
// ------------------------------------------------------------------------------

//
// _addOpenTags
//

void IndriIndex::_addOpenTags( greedy_vector<indri::index::FieldExtent>& indexedTags,
                                    greedy_vector<indri::index::FieldExtent>& openTags,
                                    const greedy_vector<TagExtent>& extents,
                                    unsigned int& extentIndex, 
                                    unsigned int position ) {
  for( ; extentIndex < extents.size(); extentIndex++ ) {
    const TagExtent* extent = &extents[extentIndex];

    if( extent->begin > position )
      break;

    int tagId = field( extent->name );

    if( tagId == 0 )
      continue;

    indri::index::FieldExtent converted( tagId, extent->begin, extent->end, extent->number );

    openTags.push_back( converted );
    indexedTags.push_back( converted );
  }
}

//
// _removeClosedTags
//

void IndriIndex::_removeClosedTags( greedy_vector<indri::index::FieldExtent>& tags, unsigned int position ) {
  for( unsigned int i=0; i<tags.size(); ) {
    if( tags[i].end <= (position + 1) ) {
      tags.erase( tags.begin() + i );
    } else {
      i++;
    }
  }
}

//
// _lookupTerm
//

void IndriIndex::_lookupTerm( const char* term, TERMID_T& termID, indri::index::TermData*& termData ) {
  // look up term in cache
  term_cache_entry** entry = _cache->find( const_cast<char*>(term) );

  if( entry ) {
    // full cache hit
    termID = (*entry)->termID;
    termData = (*entry)->termData;
  } else {
    if( _batchBuild == false ) {
      // if this isn't a batch build, we want to maintain term statistics
      // throughout the index build instead of waiting to the end to 
      // merge them.  Therefore, we're going to try to fetch statistics from
      // disk about this term
      termID = _termMap.lookupID( term );

      if( termID > 0 ) {
        termData = _fetchTermData( termID );
        _storeTermCache( term, termID, termData );
        return;
      }
    }
    
    // this is a term we haven't seen before
    _corpusStatistics.uniqueTerms++;
    termID = _corpusStatistics.uniqueTerms;

    if( _batchBuild == false ) {
      _termMap.put( termID, term );
    }

    // create a term data structure
    termData = _createTermData();

    // store termData structure in the  [termID->termData] cache
    indri::index::TermData** tableData = _termDataTable->insert( termID );
    *tableData = termData;

    // store termData and termID in the [termString->(termID, termData)] cache
    _storeTermCache( term, termID, termData );
  }
}

DOCID_T IndriIndex::addDocument( struct ParsedDocument* document ) {
  const char* documentName = "unknown";

  // find document name
  for( unsigned int i=0; i<document->metadata.size(); i++ ) {
    if( !strcmp( "docno", document->metadata[i].key ) && strlen((const char*) document->metadata[i].value) < Keyfile::MAX_KEY_LENGTH-1 ) {
      documentName = (char*) document->metadata[i].value;
      break;
    }
  }
  
  return addDocument( documentName, document->terms, document->tags );
}

DOCID_T IndriIndex::addDocument( const char* documentName, const greedy_vector<char*>& words, const greedy_vector<TagExtent>& tagExtents ) {
  unsigned int position = 0;
  unsigned int extentIndex = 0;
  greedy_vector<indri::index::FieldExtent> openTags;
  greedy_vector<indri::index::FieldExtent> indexedTags;
  unsigned int indexedTerms = 0;

  // if this index is open for reading, we can't add anything to it
  if( _readOnly ) return 0;

  // may need to make a couple of files here, if in batch mode
  _openDocumentFiles();

  // assign a document ID
  _corpusStatistics.totalDocuments++;
  DOCID_T documentID = _corpusStatistics.totalDocuments;

  _documentMap.put( documentID, documentName );
  _termList.clear();

  // move words into inverted lists, recording model statistics as we go
  for( position = 0; position < words.size(); position++ ) {
    const char* word = words[position];
    
    if( !word || *word == 0 ) {
      _termList.addTerm(0);
      continue;
    }

    int wordLength = strlen(word);

    if( wordLength >= Keyfile::MAX_KEY_LENGTH-1 ) {
      _termList.addTerm(0);     
      continue;
    }

    // fetch everything we know about this word so far
    TERMID_T wordID;
    indri::index::TermData* termData;

    _lookupTerm( word, wordID, termData );

    // store information about this term location
    _termList.addTerm( wordID );
    termData->list.addLocation( documentID, position ); 
    termData->corpus.addOccurrence( documentID );
    _seenTerms.push_back( termData );

    // update our open tag knowledge
    _addOpenTags( indexedTags, openTags, tagExtents, extentIndex, position );

    // for every open tag, we want to record that we've seen the 
    for( greedy_vector<indri::index::FieldExtent>::iterator tag = openTags.begin(); tag != openTags.end(); tag++ ) {
      indri::index::TermFieldStatistics* termField = &termData->fields[tag->id-1];
      termField->addOccurrence( documentID );

      indri::index::FieldStatistics* field = &_fieldData[tag->id-1]->statistics;
      field->addOccurrence( documentID );
    }

    _removeClosedTags( openTags, position );
    _corpusStatistics.totalTerms++;
    indexedTerms++;
  }

  // apply minimum document length and max frequency to all terms
  // this is for max-score computation at query time
  greedy_vector<indri::index::TermData*>::iterator iter;
  float oneOverDocumentLength = 1./float(words.size());

  for( iter = _seenTerms.begin(); iter != _seenTerms.end(); iter++ ) {
    indri::index::TermData* termData = *iter;
 
    if( words.size() < termData->minDocumentLength )
      termData->minDocumentLength = int(words.size());

    if( termData->corpus.lastCount > termData->maxDocumentFrequency )
      termData->maxDocumentFrequency = int(termData->corpus.lastCount);

    float fraction = float(termData->corpus.lastCount) * oneOverDocumentLength;
    if( termData->maxDocumentFraction < fraction )
      termData->maxDocumentFraction = fraction;
  }

  if( words.size() > _corpusStatistics.maximumDocumentLength ) {
    _corpusStatistics.maximumDocumentLength = words.size();
  }

  // write field data out
  for( unsigned int i=0; i<indexedTags.size(); i++ ) {
    indri::index::FieldExtent& extent = indexedTags[i];
    _termList.addField( extent );
    _fieldData[extent.id-1]->list->addExtent( documentID, extent.begin, extent.end, extent.number );
  }

  File::offset_type offset;
  int byteLength;

  _writeDocumentTermList( offset, byteLength, documentID, int(words.size()), _termList );
  _writeDocumentStatistics( offset, byteLength, indexedTerms, int(words.size()), int(_seenTerms.size()) );

  _seenTerms.clear();
  _handleCache();
  return documentID;
}

// -------------------------------------------------------------------------------------
//
// Inverted lists cache management
//
// -------------------------------------------------------------------------------------

//
// _addTermDataToBuilder
//

void IndriIndex::_addTermDataToBuilder( indri::index::DocListDiskBuilder& builder, indri::index::DocListFileIterator& iterator, int writingID, int readingID ) {
  do {
    indri::index::DocListInfo& info = iterator.currentDocument();
    DOCID_T documentID = info.docID();
    const int* positions = (const int *)info.positions();
    COUNT_T termCount = info.termCount();

    builder.addDocument( writingID, documentID, positions, termCount );
  } while( iterator.nextDocument() && iterator.currentDocument().termID() == readingID );
}

//
// _cacheSize
//

size_t IndriIndex::_cacheSize() {
  size_t totalSize = 0;

  HashTable<int, indri::index::TermData*>::iterator iter;
  int count = 0;
  size_t termDataSize = _sizeTermData();

  for( iter = _termDataTable->begin(); iter != _termDataTable->end(); iter++ ) {
    totalSize += (*iter->second)->list.memorySize();
    totalSize += termDataSize;
    count++;
  }
  assert( count == _termDataTable->size() );

  return totalSize;
}

//
// _storeTermCache
//

void IndriIndex::_storeTermCache( const char* term, TERMID_T termID, indri::index::TermData*& termData ) {
  term_cache_entry** entry = _cache->find( const_cast<char*>(term) );
  term_cache_entry* newEntry = 0;
  int termLength = strlen(term);

  newEntry = (term_cache_entry*) malloc( termLength+1 + sizeof( newEntry->termID ) + sizeof( newEntry->termData ) );
  strcpy( newEntry->term, term );

  entry = _cache->insert( const_cast<char*>(newEntry->term) );
  *entry = newEntry;

  termData->term = newEntry->term;

  (*entry)->termID = termID;
  (*entry)->termData = termData;
}

//
// _clearTermCache
//

void IndriIndex::_clearTermCache() {
  HashTable<const char*, term_cache_entry*>::iterator iter;

  for( iter = _cache->begin(); iter != _cache->end(); iter++ ) {
    free( *iter->second );
  }

  _cache->clear();
}

//
// _createTermData
//

indri::index::TermData* IndriIndex::_createTermData() {
  return termdata_create( _fieldData.size() );
}

//
// _sizeTermData
//

size_t IndriIndex::_sizeTermData() {
  return termdata_size( _fieldData.size() );
}

//
// _deleteTermData
//

void IndriIndex::_deleteTermData( indri::index::TermData* termData ) {
  termdata_delete( termData, _fieldData.size() );
}

//
// _clearTermData
//

void IndriIndex::_clearTermData() {
  HashTable<int, indri::index::TermData*>::iterator hashIter;

  for( hashIter = _termDataTable->begin(); hashIter != _termDataTable->end(); hashIter++ ) {
    _deleteTermData( *(hashIter->second) );
  }
  _termDataTable->clear();
}

//
// _cleanCache
//
 
void IndriIndex::_cleanCache() {
  HashTable<int, indri::index::TermData*>::iterator iter;
  greedy_vector<int> modified;
 
  for( iter = _termDataTable->begin(); iter != _termDataTable->end(); iter++ ) {
    if( (*iter->second)->list.empty() ) {
      modified.push_back( *iter->first );
      _deleteTermData( *(iter->second) );
    }
  }
 
  for( int i=0; i<modified.size(); i++ ) {
    _termDataTable->remove( modified[i] );
  }
}
 
//
// _fetchTermData
//

indri::index::TermData* IndriIndex::_fetchTermData( TERMID_T termID ) {
  indri::index::TermData** tableData;
  indri::index::TermData* termData = 0;
  static int fetches = 0;
  assert( termID != 0 );

  tableData = _termDataTable->find( termID );
  if( tableData )
    return *tableData;
 
  // keep cache from growing unbounded
  if( !(++fetches % 10000) ) {
    _cleanCache();
  }

  char compressedStructure[16*1024];
  int compressedLength;
  bool didFetch = _termDataStore.get( termID, compressedStructure, compressedLength, sizeof compressedStructure );
  
  if( didFetch ) {
    termData = termdata_create( _fieldData.size() );
    termdata_decompress( compressedStructure, compressedLength, _fieldData.size(), termData );

    // store termData structure in the  [termID->termData] cache
    tableData = _termDataTable->insert( termID );
    *tableData = termData;
  }

  return *tableData;
}

//
// _lookupTermData
//

indri::index::TermData* IndriIndex::_lookupTermData( TERMID_T termID ) {
  indri::index::TermData** tableData;
  assert( termID != 0 );

  tableData = _termDataTable->find( termID );
  return *tableData;
}

//
// _decompressTermData
//

void IndriIndex::_decompressTermData( const char* buffer, int size, indri::index::TermData* termData ) {
  termdata_decompress( buffer, size, _fieldData.size(), termData );
}

//
// _compressTermData
//

int IndriIndex::_compressTermData( char* buffer, int size, indri::index::TermData* termData ) {
  return termdata_compress( buffer, size, _fieldData.size(), termData );
}

//
// _batchWriteTermData
//

void IndriIndex::_batchWriteTermData( TERMID_T termID, indri::index::TermData* termData, WriteBuffer* termDataFile ) {
  char compressedStructure[ 16*1024 ]; // this is just some arbitrary buffer size that should be big enough
  int structureLength = _compressTermData( compressedStructure, 16*1024, termData );
  int termLength = strlen( termData->term );

  termDataFile->write( (char*) &termID, sizeof termID );
  termDataFile->write( (char*) &termLength, sizeof termLength );
  termDataFile->write( (char*) &structureLength, sizeof structureLength );

  termDataFile->write( termData->term, termLength );
  termDataFile->write( compressedStructure, structureLength );
}

//
// _incrementalWriteTermData
//

void IndriIndex::_incrementalWriteTermData( TERMID_T termID, indri::index::TermData* termData ) {
  char compressedStructure[ 16*1024 ]; // this is just some arbitrary buffer size that should be big enough
  int compressedSize = _compressTermData( compressedStructure, 16*1024, termData );

  _termDataStore.put( termID, compressedStructure, compressedSize );

  // TODO: debug code
  char retrievedStructure[ 16*1024 ];
  int retrievedLength = 0;
  _termDataStore.get( termID, retrievedStructure, retrievedLength, sizeof retrievedStructure );
  assert( retrievedLength == compressedSize );
  assert( !memcmp( retrievedStructure, compressedStructure, compressedSize ) );
}

//
// _readTermData
//

bool IndriIndex::_readTermData( TERMID_T& termID, char* termBuffer, indri::index::TermData* termData, ReadBuffer* termDataFile ) {
  int termLength;
  int structureLength;
  char compressedStructure[16*1024];

  if( termDataFile->rdstate() & std::fstream::eofbit )
    return false;

  termDataFile->read( (char*) &termID, sizeof termID );
  termDataFile->read( (char*) &termLength, sizeof termLength );
  termDataFile->read( (char*) &structureLength, sizeof structureLength );
  
  termDataFile->read( termBuffer, termLength );
  termBuffer[termLength] = 0;
  termDataFile->read( compressedStructure, structureLength );

  _decompressTermData( compressedStructure, structureLength, termData );
  return true;
}

// ------------------------------------------------------------------------------------------------
//
//  Batch build code
//    _writeBatchSegment
//    _mergeBatchSegments
//
// ------------------------------------------------------------------------------------------------

//
// modified_sort
//

class modified_sort {
private:
  HashTable<int, indri::index::TermData*>& _table;

public:
  modified_sort( HashTable<int, indri::index::TermData*>& table ) :
    _table(table)
  {
  }

  bool operator() ( const int& one, const int& two ) {
    indri::index::TermData** first = _table.find(one);
    indri::index::TermData** second = _table.find(two);

    return strcmp( (*first)->term, (*second)->term ) < 0;
  }
};

//
// indriindex_fetch_modified
//

void indriindex_fetch_modified( std::vector< int >& modified, HashTable<int, indri::index::TermData*>& table, bool alphaSort = false ) {
  HashTable<int, indri::index::TermData*>::iterator hashIter;

  // put all modified lists into appropriate bucket vectors
  for( hashIter = table.begin(); hashIter != table.end(); hashIter++ ) {
    indri::index::TermData* termData = (*hashIter->second);
    int termID = (*hashIter->first);

    if( ! termData->list.empty() ) {
      modified.push_back( termID );
    }
  }

  if( alphaSort ) {
    // sort each bucket in alphabetical order
    modified_sort sorter( table );
    std::sort( modified.begin(), modified.end(), sorter );
  } else {
    // sort in numerical order
    std::sort( modified.begin(), modified.end() );
  }
}

//
// indriindex_merge_termdata
//

void indriindex_merge_termdata( indri::index::TermData* master, indri::index::TermData* sub, int fields ) {
  master->corpus.documentCount += sub->corpus.documentCount;
  master->corpus.totalCount += sub->corpus.totalCount;
  
  for( int k=0; k<fields; k++ ) {
    master->fields[k].documentCount += sub->fields[k].documentCount;
    master->fields[k].totalCount += sub->fields[k].totalCount;
  }

  master->maxDocumentFraction = lemur_compat::max( sub->maxDocumentFraction, master->maxDocumentFraction );
  master->maxDocumentFrequency = lemur_compat::max( sub->maxDocumentFrequency, master->maxDocumentFrequency );
  master->minDocumentLength = lemur_compat::min( sub->minDocumentLength, master->minDocumentLength );
}

//
// _writeBatchSegment
//
// Hashes the data we know about this term into lots of buckets
// Stores the inverted list for each term in the appropriate bucket
//

void IndriIndex::_writeBatchSegment() {
  // find all of the terms that have been modified since the last cache flush, sort in alphabetical order
  std::vector<int> modified;
  indriindex_fetch_modified( modified, *_termDataTable, true );

  // if there's nothing new to report, don't write a segment
  if( !modified.size() )
    return;

  // make one output file for each hash bucket
  WriteBuffer* statsBuffer;
  File* statsFile;

  std::stringstream bucketName;
  bucketName << _baseName << ".list" << _batchSegmentCount;

  File* segment = new File();
  segment->open( bucketName.str().c_str(), std::ios::out | std::ios::binary );
  WriteBuffer* segmentBuffer = new WriteBuffer( *segment, INDRI_WRITEBUFFER_SIZE-8 );

  // build a diskBuilder object that will build the output file
  indri::index::DocListDiskBuilder* diskBuilder = new indri::index::DocListDiskBuilder( *segmentBuffer );

  // build stats file
  std::stringstream statsName;
  statsName << _baseName << ".stats" << _batchSegmentCount;
  statsFile = new File();
  statsFile->open( statsName.str().c_str(), std::ios::out | std::ios::binary );
  statsBuffer = new WriteBuffer( *statsFile, (INDRI_WRITEBUFFER_SIZE/4)-8 );
  
  // build termID lookup
  greedy_vector<int> termMapping;
  termMapping.resize(_corpusStatistics.uniqueTerms+1);
  termMapping[0] = 0;

  // start writing data
  int alphabeticID = 1;

  for( size_t i=0; i<modified.size(); i++ ) {
    int termID = modified[i];
    indri::index::TermData* termData = _lookupTermData( termID );
    indri::index::DocListMemoryBuilder* list = &termData->list;
    
    termMapping[termID] = alphabeticID;

    list->close();

    // copy into diskBuilder, one entry at a time
    indri::index::DocListMemoryBuilder::iterator iter = list->getIterator();

    while( iter.next() ) {
      diskBuilder->addLocation( alphabeticID, iter.document(), iter.position() );
    }

    // could simply reduce the allocation here instead of dumping the
    // buffer entirely, but it probably makes sense to release all the
    // inverted list memory to defrag the heap a little
    list->clear();

    // write term data out here
    _batchWriteTermData( alphabeticID, termData, statsBuffer );
    alphabeticID++;
  }

  // build mapping file
  std::stringstream mappingName;
  mappingName << _baseName << ".alphamap" << _batchSegmentCount;
  File* mappingFile = new File();
  mappingFile->open( mappingName.str().c_str(), std::ios::out | std::ios::binary );
  mappingFile->write( &termMapping.front(), termMapping.size() * sizeof(int) );
  mappingFile->close();
  delete mappingFile;

  // close up all the files and buffers
  diskBuilder->flush();
  segmentBuffer->flush();
  segment->close();
  delete diskBuilder;
  delete segmentBuffer;
  delete segment;

  statsBuffer->flush();
  statsFile->close();
  delete statsBuffer;
  delete statsFile;

  _clearTermData();
  _clearTermCache();

  _batchSegmentCount++;
  _corpusStatistics.uniqueTerms = 0;

  _documentStatisticsFile->close();
  delete _documentStatisticsFile;
  _documentStatisticsFile = 0;

  _documentTermLocationsFile->close();
  delete _documentTermLocationsFile;
  _documentTermLocationsFile = 0;
}


void indriindex_get_smallest( greedy_vector<int>& smallest, std::vector<char*>& terms ) {
  smallest.clear();

  // find smallest term
  for( unsigned int j=0; j<terms.size(); j++ ) {
    if( !terms[j] )
      continue;

    if( !smallest.size() ) {
      smallest.push_back(j);
      continue;
    }
    
    int compare = strcmp( terms[smallest[0]], terms[j] );

    if( compare > 0 )
      smallest.clear();

    if( compare >= 0 )
      smallest.push_back(j);
  }
}

void IndriIndex::_openMergeFiles( int startSegment,
                                         int endSegment,
                                         std::vector<File*>& listFiles,
                                         std::vector<File*>& statsFiles,
                                         std::vector<File*>& mappingFiles,
                                         std::vector<WriteBuffer*>& mappingBuffers,
                                         std::vector<ReadBuffer*>& statsBuffers,
                                         std::vector<indri::index::DocListFileIterator*>& listIterators,
                                         std::vector<char*>& terms,
                                         std::vector<indri::index::TermData*>& termDatas,
                                         bool finalMerge )
{
  const char* mapSuffix = finalMerge ? ".expandedmap" : ".intermap";

  // for each segment, open all necessary files and read in statistics/termID data
  for( int j=startSegment; j<endSegment; j++ ) {
    // open inv file
    std::stringstream listName;
    listName << _baseName << ".list" << j;
    
    File* listFile = new File();
    listFile->open( listName.str().c_str(), std::ios::in | std::ios::binary );
    if( listFile->rdstate() & std::ios::failbit ) {
      // file doesn't exist
      delete listFile;
      continue;
    }

    listFiles.push_back( listFile );

    indri::index::DocListFileIterator* listIterator = new indri::index::DocListFileIterator( listFile, j, INDRI_MERGE_READBUFFER_SIZE );
    listIterators.push_back( listIterator );
    listIterator->startIteration();
    listIterator->nextDocument();
    
    // open stats
    std::stringstream statsName;
    statsName << _baseName << ".stats" << j;

    File* statsFile = new File();
    statsFile->open( statsName.str().c_str(), std::ios::in | std::ios::binary );
    statsFiles.push_back( statsFile );

    ReadBuffer* statsBuffer = new ReadBuffer( *statsFile, INDRI_MERGE_TERMLISTBUFFER_SIZE );
    statsBuffers.push_back( statsBuffer );

    // mapping file
    std::stringstream mappingName;
    mappingName << _baseName << mapSuffix << j;

    File* mappingFile = new File();
    mappingFile->open( mappingName.str().c_str(), std::ios::out | std::ios::binary );
    mappingFiles.push_back( mappingFile );
    mappingBuffers.push_back( new WriteBuffer( *mappingFile, 128*1024 ) );
    int zero = 0;
    mappingBuffers.back()->write( (const char*) &zero, sizeof zero );

    // set up buffers
    terms.push_back( new char[Keyfile::MAX_KEY_LENGTH+1] );
    termDatas.push_back( termdata_create( _fieldData.size() ) );

    // read first entry
    TERMID_T bogusTermID;
    bool didRead = _readTermData( bogusTermID, terms.back(), termDatas.back(), statsBuffers.back() );

    if( !didRead ) {
      delete terms.back();
      delete termDatas.back();
      terms.back() = 0;
      termDatas.back() = 0;
    }
  }
}

static void indriindex_close_merge_files( std::vector<File*>& listFiles,
                                          std::vector<File*>& mappingFiles,
                                          std::vector<File*>& statsFiles,
                                          std::vector<WriteBuffer*>& mappingBuffers,
                                          std::vector<ReadBuffer*>& statsBuffers,
                                          std::vector<indri::index::DocListFileIterator*>& listIterators ) {
  for( unsigned int j=0; j<listFiles.size(); j++ ) {
    mappingBuffers[j]->flush();
    delete mappingBuffers[j];
    mappingFiles[j]->close();
    delete mappingFiles[j];

    delete listIterators[j];
    listFiles[j]->unlink();
    delete listFiles[j];

    delete statsBuffers[j];
    statsFiles[j]->unlink();
    delete statsFiles[j];
  }
}

//
// _mergeBatchSegments
//

void IndriIndex::_mergeBatchSegments( int start, int end, int newNumber, bool finalMerge ) {
  assert( _batchBuild && !_readOnly );

  // make sure we don't try to merge if nothing has changed
  if( _batchSegmentCount == 0 )
    return;

  _corpusStatistics.uniqueTerms = 0;

  File outFile;
  std::string ivlFileName;
  if( finalMerge )
    ivlFileName = _buildFileName( IVLINDEX, 0 );
  else 
    ivlFileName = _buildFileName( ".list", newNumber );
  outFile.open( ivlFileName.c_str(), std::ofstream::out | std::ofstream::binary );
  WriteBuffer out(outFile, INDRI_WRITEBUFFER_SIZE);
  indri::index::DocListDiskBuilder diskBuilder( out );

  std::vector<File*> listFiles;
  std::vector<File*> statsFiles;
  std::vector<File*> mappingFiles;
  std::vector<WriteBuffer*> mappingBuffers;
  std::vector<ReadBuffer*> statsBuffers;
  std::vector<indri::index::DocListFileIterator*> listIterators;
  std::vector<char*> terms;
  std::vector<indri::index::TermData*> termDatas;

  _openMergeFiles( start, end,
                   listFiles, statsFiles, mappingFiles, mappingBuffers,
                   statsBuffers, listIterators, terms, termDatas, finalMerge );
  greedy_vector<int> smallest;

  WriteBuffer* interStatsBuffer = 0;
  File* interStatsFile = 0;
  
  if( !finalMerge ) {
    std::string interName = _buildFileName( ".stats", newNumber );
    interStatsFile = new File();
    interStatsFile->open( interName, std::ios::binary | std::ios::out );
    interStatsBuffer = new WriteBuffer( *interStatsFile, INDRI_WRITEBUFFER_SIZE);
  }

  while(1) {
    indriindex_get_smallest( smallest, terms );

    if( !smallest.size() ) {
      // no smallest term, so we're done
      break;
    }

    // put term into the keyfiles
    TERMID_T termID = ++_corpusStatistics.uniqueTerms;
    _termMap.put( termID, terms[smallest[0]] );

    indri::index::TermData* termData = termdata_create( _fieldData.size() );
    std::string termDataTerm = terms[smallest[0]];
    termData->term = termDataTerm.c_str();

    // save the beginning block for indexing purposes
    File::offset_type startOffset = diskBuilder.currentBlock();

    // need to merge data for all indexes in smallest
    for( size_t j=0; j<smallest.size(); j++ ) {
      int index = smallest[j];
      indri::index::DocListFileIterator* listIterator = listIterators[index];
    
      // copy from iterator into the builder (until termID data is done)
      _addTermDataToBuilder( diskBuilder, *listIterator, termID, listIterator->currentDocument().termID() );

      // merge in term data
      indri::index::TermData* listTermData = termDatas[index];
      indriindex_merge_termdata( termData, listTermData, _fieldData.size() );
      
      // write out the termID mapping
      mappingBuffers[index]->write( (const char*) &termID, sizeof termID );

      // advance to next entry
      TERMID_T alphaTermID;
      bool didRead = _readTermData( alphaTermID, terms[index], termDatas[index], statsBuffers[index] );

      if( !didRead ) {
        delete[] terms[index];
        delete termDatas[index];
        terms[index] = 0;
        termDatas[index] = 0;
      }
    }

    // put position into term data
    termData->segmentOffsets[0] = startOffset;

    // flush term data
    if( finalMerge )
      _incrementalWriteTermData( termID, termData );
    else
      _batchWriteTermData( termID, termData, interStatsBuffer );
    termdata_delete( termData, _fieldData.size() );
  }

  indriindex_close_merge_files( listFiles, mappingFiles, statsFiles, mappingBuffers, statsBuffers, listIterators );
  
  if( interStatsBuffer ) {
    interStatsBuffer->flush();
    delete interStatsBuffer;
    interStatsFile->close();
    delete interStatsFile;
  }

  diskBuilder.flush();
  out.flush();
  outFile.close();
}

//
// This method is only used in batch mode.
//
// The first map (alphamap) maps the
// initial term IDs (in term arrival order) to the segment termID (in segment
// alphabetical order).  The second map maps segment termIDs to the merged
// termIDs (also in alphabetical order, but 'expanded', because there are
// terms in the merged representation that aren't in a given segment).
//
// This method opens both files and puts the mappings together, then
// deletes the mapping files.
//

void indriindex_read_mapping( const std::string& mappingName, greedy_vector<int>& mapping, bool del ) {
  File f;
  f.open( mappingName, std::ios::in | std::ios::binary );

  if( f.rdstate() & std::ios::failbit )
    return;

  File::offset_type mappingSize = f.size();
  mapping.resize( size_t(mappingSize/sizeof(int)) );
  f.read( &mapping.front(), mappingSize );
  f.close();
  if( del )
    f.unlink();
}

void IndriIndex::_readTermMapping( greedy_vector<int>& mapping, int segment, int secondSegment ) {
  greedy_vector<int> firstMap;
  greedy_vector<int> interMap;
  greedy_vector<int> secondMap;

  mapping.clear();

  std::stringstream firstMappingName;
  firstMappingName << _baseName << ".alphamap" << segment;
  indriindex_read_mapping( firstMappingName.str(), firstMap, true );

  std::stringstream interMapName;
  interMapName << _baseName << ".intermap" << segment;
  indriindex_read_mapping( interMapName.str(), interMap, true );

  std::stringstream secondMappingName;
  secondMappingName << _baseName << ".expandedmap" << secondSegment;
  indriindex_read_mapping( secondMappingName.str(), secondMap, false );

  mapping.resize( firstMap.size() );

  if( !interMap.size() ) {
    for( size_t i=0; i<mapping.size(); i++ ) {
      mapping[i] = secondMap[firstMap[i]];
    }
  } else {
    for( size_t i=0; i<mapping.size(); i++ ) {
      mapping[i] = secondMap[interMap[firstMap[i]]];
    }
  }
}

void IndriIndex::_mergeBatchTermLists( const std::vector<int>& segmentMapping ) {
  assert( _batchBuild && !_readOnly );
  Buffer termListOutputBuffer;

  _documentStatisticsFile = new File();
  std::string documentStatsName = _buildFileName( DOCSTATS );
  _documentStatisticsFile->open( documentStatsName, std::ios::out );

  _documentTermLocationsFile = new File();
  std::string docIndexName = _buildFileName( DOCINDEX );
  _documentTermLocationsFile->open( docIndexName, std::ios::out );

  for( int i=0; i<segmentMapping.size(); i++ ) {
    // read in mapping
    greedy_vector<int> mapping;
    _readTermMapping( mapping, i, segmentMapping[i] );

    // open up the necessary files
    std::string segmentStatisticsName = _buildFileName( DOCSTATSBATCH, i );
    File segmentStatistics;
    segmentStatistics.open( segmentStatisticsName, std::ios::in );
    ReadBuffer statisticsBuffer( segmentStatistics, INDRI_MERGE_TERMLISTBUFFER_SIZE );

    std::string segmentDocIndexName = _buildFileName( DOCINDEXBATCH, i );
    File segmentDocIndex;
    segmentDocIndex.open( segmentDocIndexName, std::ios::in );
    ReadBuffer indexBuffer( segmentDocIndex, INDRI_MERGE_TERMLISTBUFFER_SIZE );

    indri::index::TermListBuilder termList;

    while( !( statisticsBuffer.rdstate() & std::ios::eofbit ) ) {
      // read statistics, get length and offset
      // read list data
      indri::index::DocumentData documentData;
      statisticsBuffer.read( (char*) &documentData, sizeof documentData );

      const char* listData = indexBuffer.read( documentData.byteLength );
      termList.read( listData, documentData.byteLength );

      greedy_vector<int>& terms = termList.terms();
      for( size_t j=0; j<terms.size(); j++ ) {
        terms[j] = mapping[terms[j]];
      }

      termList.write( termListOutputBuffer );
      File::offset_type offset = _documentTermLocationsFile->tellp();
      _documentTermLocationsFile->write( termListOutputBuffer.front(), termListOutputBuffer.position() );
      
      documentData.offset = offset;
      documentData.byteLength = termListOutputBuffer.position();
      _documentStatisticsFile->write( &documentData, sizeof documentData );
    }

    segmentDocIndex.unlink();
    segmentStatistics.unlink();
  }

  // delete all secondMaps, since they aren't deleted at merge time
  for( int i=0; i<segmentMapping.size(); i++ ) {
    std::stringstream segmentNameStream;
    segmentNameStream << _baseName << ".expandedmap" << segmentMapping[i];
    std::string segmentName = segmentNameStream.str();
    File::unlink(segmentName);
  }
}

// --------------------------------------------------------------------
//
//  Incremental segment management
//     _writeIncrementalSegment()
//     _mergeIncrementalSegments()
//
// --------------------------------------------------------------------

void IndriIndex::_writeIncrementalSegment() {
  // find all of the terms that have been modified since the last cache flush, sort in termID order
  std::vector<int> modified;
  indriindex_fetch_modified( modified, *_termDataTable, false );

  // if there's nothing new to report, don't write a segment
  if( !modified.size() )
    return;

  int segmentNumber = _segments.size();

  File outFile;
  std::string oldName = _buildFileName( IVLINDEX, segmentNumber );
  outFile.open( oldName.c_str(), std::ofstream::out | std::ofstream::binary );
  WriteBuffer out(outFile, INDRI_WRITEBUFFER_SIZE);
  indri::index::DocListDiskBuilder diskBuilder( out );

  for( size_t i=0; i<modified.size(); i++ ) {
    int termID = modified[i];
    indri::index::TermData* termData = _lookupTermData( termID );
    indri::index::DocListMemoryBuilder* list = &termData->list;
    list->close();
    File::offset_type startOffset = diskBuilder.currentBlock();

    // copy into diskBuilder, one entry at a time
    indri::index::DocListMemoryBuilder::iterator iter = list->getIterator();

    while( iter.next() ) {
      diskBuilder.addLocation( termID, iter.document(), iter.position() );
    }

    // could simply reduce the allocation here instead of dumping the
    // buffer entirely, but it probably makes sense to release all the
    // inverted list memory to defrag the heap a little
    list->clear();

    // put position into term data
    termData->segmentOffsets[segmentNumber] = startOffset;

    // write term data out here
    _incrementalWriteTermData( termID, termData );
  }

  _clearTermData();
  _clearTermCache();

  // clean up the structures
  diskBuilder.flush();
  out.flush();
  outFile.close();

  // re-open this segment as a read stream
  File* inputSegment = new File();
  inputSegment->open( oldName, std::ios::in | std::ios::binary );
  _segments.push_back( inputSegment );
}

void IndriIndex::_mergeIncrementalSegments() {
  std::vector<indri::index::DocListFileIterator*> fileIterators;

  // only need to merge multiple segments
  if( _segments.size() <= 1 )
    return;

  // build some file iterators
  for( unsigned int i=0; i<_segments.size(); i++ ) {
    indri::index::DocListFileIterator* iterator = new indri::index::DocListFileIterator( _segments[i], i, INDRI_MERGE_READBUFFER_SIZE );
    fileIterators.push_back(iterator);
    iterator->startIteration();
    iterator->nextDocument();
  }

  // build an output iterator
  File outFile;
  std::string oldName = _buildFileName( IVLINDEX, _segments.size() );
  outFile.open( oldName.c_str(), std::ofstream::out | std::ofstream::binary );
  WriteBuffer out(outFile, INDRI_WRITEBUFFER_SIZE);
  indri::index::DocListDiskBuilder diskBuilder( out );

  // the common data structure choice here would be a priority queue, but
  // with the small number of objects available (under 8), linear search
  // does pretty well in the worst case and goes faster in the common case (<<8)
  greedy_vector<int> indexes;

  while(1) {
    indexes.clear();

    int smallTermID = MAX_INT32;

    // find the smallest termID, then find all segments that
    // contain that termID
    for( unsigned int i=0; i<fileIterators.size(); i++ ) {
      if( !fileIterators[i] )
        continue;

      int thisID = fileIterators[i]->termID();
      
      if( thisID < smallTermID ) {
        smallTermID = thisID;
        indexes.clear();
      }

      if( thisID == smallTermID ) {
        indexes.push_back(i);
      }
    }

    if( !indexes.size() )
      break; // no indexes left, so we're finished

    // write out a new term data structure
    File::offset_type startOffset = diskBuilder.currentBlock();
    indri::index::TermData* termData = _fetchTermData( smallTermID );
    memset( termData->segmentOffsets, 0xff, sizeof termData->segmentOffsets );

    termData->segmentOffsets[0] = startOffset;
    _incrementalWriteTermData( smallTermID, termData );

    if( _termDataTable->size() > 100000 ) {
      _clearTermData();
      _clearTermCache();
    }

    // combine all the inverted list stuff into one master list
    for( unsigned int index=0; index<indexes.size(); index++ ) {
      unsigned int i = indexes[index];
      _addTermDataToBuilder( diskBuilder, *fileIterators[i], smallTermID, smallTermID ); 

      if( fileIterators[i]->finished() ) {
        fileIterators[i]->remove();
        delete fileIterators[i];
        fileIterators[i] = 0;
      }
    }
  }

  _clearTermData();
  _clearTermCache();

  // close file structures
  diskBuilder.flush();
  out.flush();
  outFile.close();

  // move the master segment
  std::string newName = _buildFileName( IVLINDEX, 0 );
  File::rename( oldName, newName );

  // re-open the master segment
  File* newFile = new File();
  newFile->open( newName, std::ios::in | std::ios::binary );
  _segments.clear();
  _segments.push_back(newFile);
}

void IndriIndex::_writeCache() {
  if( _batchBuild ) {
    _writeBatchSegment();
  } else {
    _writeIncrementalSegment();

    if( _segments.size() >= INDRI_MAX_SEGMENTS ) {
      _mergeIncrementalSegments();
    }
  }
}

void IndriIndex::_mergeBatch() {
  assert( _batchBuild );
  std::vector<int> segmentMap;

  if( _batchSegmentCount > INDRI_MAX_BATCH_MERGE_SEGMENTS ) {
    // can't open this many filehandles at once, so we'll do a two-stage 
    int averageMerge = (int) ceil( double(_batchSegmentCount) / INDRI_MAX_BATCH_MERGE_SEGMENTS );
    int mergeWidth = std::max<int>( averageMerge, 8 );
    int merges = 0;

    // merge chunks of segments, storing the new segments with new numbers
    // we'll put the new numbers in a mapping vector so that the mergeBatchTermLists call can find them
    for( int i=0; i<_batchSegmentCount-mergeWidth; i+=mergeWidth, merges++ ) {
      _mergeBatchSegments( i, i+mergeWidth, _batchSegmentCount+merges, false );
      for( int j=0; j<mergeWidth; j++ )
        segmentMap.push_back( _batchSegmentCount+merges );
    }

    while( segmentMap.size() < _batchSegmentCount )
      segmentMap.push_back(segmentMap.size());

    _mergeBatchSegments( 0, _batchSegmentCount+merges, 0, true );
  } else {
    for( int i=0; i<_batchSegmentCount; i++ )
      segmentMap.push_back(i);

    _mergeBatchSegments( 0, _batchSegmentCount, 0, true );
  }

  _mergeBatchTermLists( segmentMap );
  _batchBuild = false;
  _batchSegmentCount = 0;
}

void IndriIndex::_writeAndMerge() {
  if( _batchBuild ) {
    _writeBatchSegment();
    _mergeBatch();
  } else {
    _writeIncrementalSegment();
    _mergeIncrementalSegments();
  }
}

// -----------------------------------------------------------------------------
//
// Document management
//
// -----------------------------------------------------------------------------

//
// _writeDocumentTermList
//
// Writes information about the position of every term in a 
// document into a file.  The _writeDocumentStatistics stores
// the offset of this term list so that it can be found later.
//

void IndriIndex::_writeDocumentTermList( File::offset_type& offset, int& byteLength, DOCID_T documentID, int documentLength, indri::index::TermListBuilder& locatedTerms ) {
  offset = _documentTermLocationsFile->tellp();
  
  if( _writingDocTermLists ) {
    _termList.write( _termListBuffer );
    _documentTermLocationsFile->write( _termListBuffer.front(), _termListBuffer.position() );
    byteLength = _termListBuffer.position();
  }
}

void IndriIndex::_openDocumentFiles() {
  if( !_documentStatisticsFile ) {
    assert( _batchBuild );
    std::string statsFileName = _buildFileName( DOCSTATSBATCH, _batchSegmentCount );
    _documentStatisticsFile = new File();
    _documentStatisticsFile->open( statsFileName, std::ios::out );
  }

  if( !_documentTermLocationsFile ) {
    assert( _batchBuild );
    std::string docTermName = _buildFileName( DOCINDEXBATCH, _batchSegmentCount );
    _documentTermLocationsFile = new File();
    _documentTermLocationsFile->open( docTermName, std::ios::out );
  }
}

//
// _writeDocumentStatistics
//
// Writes information about a document's length, number of unique terms,
// and term list offset into a file for quick access
//

void IndriIndex::_writeDocumentStatistics( File::offset_type offset, int byteLength, int indexedLength, int totalLength, int uniqueTerms ) {
  indri::index::DocumentData data;

  data.offset = offset;
  data.byteLength = byteLength;
  data.indexedLength = indexedLength;
  data.uniqueTermCount = uniqueTerms;

  _documentStatisticsFile->write( (char*) &data, sizeof data );
  _documentLengthFile.write( (char*) &totalLength, sizeof totalLength );
}

//
// _handleCache
//
// Writes data to cache if the cache is deemed to be full
//

void IndriIndex::_handleCache() {
  bool shouldWriteCache = false;

  // check to see if we're at an estimated flush point
  if( _corpusStatistics.totalTerms > _estimatePoint ) {
      INT64 termsSinceLastFlush = _corpusStatistics.totalTerms - _lastCacheFlush;
      size_t cacheSize = _cacheSize();
      float bytesPerPosting = (float)cacheSize / (float)termsSinceLastFlush;
      _estimatePoint = _lastCacheFlush + (size_t) (_listsSize / bytesPerPosting);

      // if we're within a few postings of an optimal flush point, just do it now
      shouldWriteCache = _estimatePoint < (_corpusStatistics.totalTerms + INDRI_EPSILON_FLUSH_POSTINGS);
  }

  // if we've decided to write the cache... do it
  if( shouldWriteCache ) {
    _writeCache();
    _resetEstimatePoint();
  }
}

// ----------------------------------------------------------------------------------
//
// Index helper methods
//
// ----------------------------------------------------------------------------------

indri::index::DocumentData IndriIndex::fetchDocumentData( int key ) const {
  assert( key <= _corpusStatistics.totalDocuments );
  assert( key >= 0 );

  if (key == 0)
    return indri::index::DocumentData();

  indri::index::DocumentData data;

  _documentStatisticsBuffer->seekg( (key-1)*sizeof(indri::index::DocumentData), std::fstream::beg );
  _documentStatisticsBuffer->read( (char*) &data, sizeof(indri::index::DocumentData) );

  return data;
}

int IndriIndex::fetchDocumentLength( int key ) const {
  assert( key <= _corpusStatistics.totalDocuments );
  assert( key >= 0 );

  if (key == 0)
    return 0;

  int length;

  _documentLengthBuffer->seekg( (key-1)*sizeof(int), std::fstream::beg );
  _documentLengthBuffer->read( (char*) &length, sizeof(int) );

  return length;
}

