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

/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef INDRI_INDEX_HPP
#define INDRI_INDEX_HPP

/*
 * NAME DATE - COMMENTS
 * tnt 01/02 - created
 * dmf 07/03 - converted to incremental berkeley db btree indexer with
 * integrated document manager.
 * tds 09/03 - modified from BTIncIndex to use keyfile
*/

#include "TermInfoList.hpp"
#include "DocInfoList.hpp"
#include "lemur-platform.h"
#include "lemur-compat.hpp"
#include "indri/DocPositionInfoList.hpp"
#include "Exception.hpp"
#include "Keyfile.hpp"
#include "ReadBuffer.hpp"
#include "WriteBuffer.hpp"

#include <queue>
#include <map>

#include "indri/DocListMemoryBuilder.hpp"
#include "indri/DocListDiskBuilder.hpp"
#include "indri/HashTable.hpp"
#include <indri/greedy_vector>

#include "indri/TermData.hpp"
#include "indri/TermFieldStatistics.hpp"
#include "indri/KeyfileWordMap.hpp"
#include "indri/DocumentData.hpp"

#include "indri/CorpusStatistics.hpp"
#include "indri/FieldStatistics.hpp"
#include "indri/FieldData.hpp"
#include "indri/TermListBuilder.hpp"
#include "indri/FieldListIterator.hpp"
#include "indri/DocListFrequencyIterator.hpp"

#include "indri/TagExtent.hpp"


struct ParsedDocument;

#define INDRI_DEFAULT_QUERY_PROPORTION     (0.5)
#define INDRI_DEFAULT_MEMORY_SIZE          (128*1024*1024)
#define INDRI_OUTRAGEOUS_DOCLENGTH         (1000*1000*1000)

struct less_string {
  bool operator () ( const char* one, const char* two ) const {
    return strcmp( one, two ) < 0;
  }
};

/*! Indri internal index class. 
 */
class IndriIndex {
private:
  struct term_cache_entry {
    TERMID_T termID;
    indri::index::TermData* termData;
    char term[0];
  };

public:
  /// Field data
  struct FieldDescription {
    /// name of the field
    std::string name;
    /// does the field contain numeric data
    bool numeric;
  };
  /// Create.
  /// @param memorySize how much memory to use
  /// @param queryProportion proportion of load assumed to be queries
  IndriIndex( size_t memorySize = INDRI_DEFAULT_MEMORY_SIZE, float queryProportion = INDRI_DEFAULT_QUERY_PROPORTION);  
  ~IndriIndex();

  /// sets the name for this index
  void setName(const std::string& prefix);
  /// add a parsed document to the index.
  /// @param document the document to add
  /// @return the internal document id of the document
  DOCID_T addDocument( struct ParsedDocument* document );

  // still support this call in the transition period
  /// add a parsed document to the index.
  /// @param documentName the name of the document to add
  /// @param words greedy vector of the terms in the document
  /// @param tagExtents greedy vector of the tag extents for the document.
  /// @return the internal document id of the document
  DOCID_T addDocument( const char* documentName, const greedy_vector<char*>& words, const greedy_vector<TagExtent>& tagExtents );

  /// @name Open index 
  //@{

  /// Open previously created Index with given prefix
  bool open(const std::string& indexName);
  /// Open previously created Index with given prefix
  bool open( const char* indexName );
  /// Open previously created Index with given prefix in read only mode
  bool openRead(const std::string& indexName);


  /// Create a new index with the given prefix
  bool create(const std::string& indexName );

  /// Create a new index with the given prefix and tag set
  bool create(const std::string& indexName, const std::vector<FieldDescription>& fields );
  /// Close the index
  void close();

  //@}

  /// @name Spelling and index conversion
  //@{

  /// Convert a term spelling to a termID
  TERMID_T term(const TERM_T &word) const;

  /// Convert a termID to its spelling
  const TERM_T term(TERMID_T termID) const;

  /// Convert a spelling to docID
  DOCID_T document(const EXDOCID_T &docIDStr) const;

  /// Convert a docID to its spelling
  const EXDOCID_T document(DOCID_T docID) const; 

  /// Convert a fieldID to its name
  const char* field(int fieldID);

  /// Convert a field name to its fieldID
  int field( const char* fieldName );

  /// Convert a field name to its fieldID
  int field( const std::string& fieldName );

  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  COUNT_T docCount() const { return _corpusStatistics.totalDocuments; };

  /// Total count of unique terms in collection
  COUNT_T termCountUnique() const { return _corpusStatistics.uniqueTerms; };

  /// Total counts of a term in collection
  INT64 termCount(TERMID_T termID) const;

  /// Total counts of all terms in collection
  INT64 termCount() const { return _corpusStatistics.totalTerms; };

  /// Total counts of a term in a field
  INT64 fieldTermCount(int fieldID, TERMID_T termID) const;

  /// Total counts of all terms in a field
  INT64 fieldTermCount(int fieldID) const;

  /// Total count of documents that contain a given field
  INT64 fieldDocCount(int fieldID) const;

  /// Total count of documents that contain a given term in a given field
  INT64 fieldDocCount(int fieldID, TERMID_T termID) const;

  /// Average document length 
  double docLengthAvg() const;

  /// Total counts of doc with a given term
  COUNT_T docCount(TERMID_T termID) const;
  /// return indexed length of the document
  COUNT_T docIndexedLength( DOCID_T documentID ) const;
  /// return length of the document
  COUNT_T docLength( DOCID_T documentID ) const;

  /// Maximum number of times this term is in any documents
  int termMaxDocumentFrequency( TERMID_T termID );

  /// Minimum length of any document containing this term
  int termMinDocumentLength( TERMID_T termID );

  /// Argmax over documents of (termCount/documentLength)
  double termMaxDocumentFraction( TERMID_T termID );

  /// Maximum length of any document in the corpus
  int maxDocumentLength();

  //@}

  /// doc entries in a term index, @see DocList
  DocInfoList* docInfoList(TERMID_T termID) const;
  /// doc entries in a term index with positions
  DocPositionInfoList* docPositionInfoList(TERMID_T termID);
  /// doc entries in a term index without positions
  indri::index::DocListFrequencyIterator* docFrequencyInfoList(TERMID_T termID);

  /// word entries in a document index (bag of words), @see TermList
  TermInfoList* termInfoList(DOCID_T docID) const;
  /// word entries in a document index (sequence of words), @see TermList
  TermInfoList* termInfoListSeq(DOCID_T docID) const;

  /// internal IndriIndex term list representation
  indri::index::TermListBuilder* termPositionList(DOCID_T docID);

  /// field list
  indri::index::FieldListIterator* fieldPositionListIterator( int fieldID );

protected:
  bool _readOnly;

  void _writeCache();
  void _writeAndMerge();

  void _writeBatchSegment();
  void _mergeBatch();
  void _mergeBatchSegments( int start, int end, int newNumber, bool finalMerge );
  void _mergeBatchTermLists( const std::vector<int>& segmentMapping );
  
  void _writeIncrementalSegment();
  void _mergeIncrementalSegments();

  void _readTermMapping( greedy_vector<int>& mapping, int segment, int secondSegment );

  void _openMergeFiles( int startSegment,
                        int endSegment,
                        std::vector<File*>& listFiles,
                        std::vector<File*>& statsFiles,
                        std::vector<File*>& mappingFiles,
                        std::vector<WriteBuffer*>& mappingBuffers,
                        std::vector<ReadBuffer*>& statsBuffers,
                        std::vector<indri::index::DocListFileIterator*>& listIterators,
                        std::vector<char*>& terms,
                        std::vector<indri::index::TermData*>& termDatas,
                        bool finalMerge );

  void _openDBs();
  void _openReadOnlyDBs();
  void _openSegments();
  void _createDBs();
  void _createFields( const std::vector<FieldDescription>& fieldNames );
  void _closeFields();

  indri::index::DocumentData fetchDocumentData( int key ) const;
  int fetchDocumentLength( int key ) const;

  void _updateTermlist( TERMID_T termID, int position );
  int _updateTermData( int documentLength );
  size_t _cacheSize();
  void _computeMemoryBounds( size_t memorySize, float queryProportion );
  void _resetEstimatePoint();

  // special handling for term data, since it varies in size
  // based on the number of indexed fields
  indri::index::TermData* _createTermData();
  indri::index::TermData* _fetchTermData(TERMID_T termID);
  indri::index::TermData* _lookupTermData(TERMID_T termID);
  void _cleanCache();
  void _deleteTermData( indri::index::TermData* termData );
  size_t _sizeTermData();
  void _clearTermData();

  // special handling for cache data
  void _clearTermCache();
  void _storeTermCache( const char* term, TERMID_T termID, indri::index::TermData*& termData );

  void _flushTermStatistics( TERMID_T termID, const indri::index::TermFieldStatistics& statistics );
  void _addTermDataToBuilder( indri::index::DocListDiskBuilder& builder, indri::index::DocListFileIterator& iterator, int writingID, int readingID );

  // addDocument helpers
  void _addOpenTags( greedy_vector<indri::index::FieldExtent>& indexedTags,
                     greedy_vector<indri::index::FieldExtent>& openTags,
                     const greedy_vector<TagExtent>& extents,
                     unsigned int& extentIndex,
                     unsigned int position );
  void _removeClosedTags( greedy_vector<indri::index::FieldExtent>& tags, unsigned int position );
  void _lookupTerm( const char* term, TERMID_T& termID, indri::index::TermData*& termData );
  void _finishDocument( greedy_vector<indri::index::TermFieldStatistics*>& seenStatistics );
  void _writeDocumentTermList( File::offset_type& offset, int& byteLength, DOCID_T documentID, int documentLength, indri::index::TermListBuilder& locatedTerms );
  void _writeDocumentStatistics( File::offset_type offset, int byteLength, int indexedLength, int totalLength, int uniqueTerms );
  void _handleCache();
  int _lookupTag( const char* tag );

  bool _readTermData( TERMID_T& termID, char* termBuffer, indri::index::TermData* termData, ReadBuffer* termDataFile );
  void _incrementalWriteTermData( TERMID_T termID, indri::index::TermData* termData );
  void _batchWriteTermData( TERMID_T termID, indri::index::TermData* termData, WriteBuffer* file );
  int _compressTermData( char* buffer, int size, indri::index::TermData* termData );
  void _decompressTermData( const char* buffer, int size, indri::index::TermData* termData );

  void _writeParameters( const std::string& fileName );
  bool _readParameters( const std::string& fileName );

  
  void _openDocumentFiles();
  std::string _buildFileName( const char* suffix );
  std::string _buildFileName( const char* suffix, int index );

  // count statistics
  indri::index::CorpusStatistics _corpusStatistics;
  std::vector<indri::index::FieldData*> _fieldData;
  std::map<const char*, int, less_string> _fieldLookup;

  // General index parameters and state
  std::string _baseName;    /// the prefix name
  bool _writingDocTermLists;

  // ---- Disk-based structures --------------------

  // All database handles are marked mutable since they sometimes
  // must be used to fetch values during const methods
  mutable Keyfile _termDataStore;         // termID -> indri::index::TermData (term statistics and inverted list segment offsets)
  mutable KeyfileWordMap _documentMap;    // documentID <-> documentString
  mutable KeyfileWordMap _termMap;        // termID <-> termString
  mutable File* _documentStatisticsFile;  // document statistics (document length, etc.)
  mutable File _documentLengthFile;       // document length only

  std::vector<File*> _segments;           // inverted list segment files
  int _batchSegmentCount;                 // count of segments
  File* _documentTermLocationsFile;       // filestream for writing the list of located terms

  // ---- Current document structures --------------
  //    (since IndriIndex is now doc-at-a-time, the main reason for keeping
  //    these here is to reduce allocation overhead)

  indri::index::TermListBuilder _termList;
  Buffer _termListBuffer;

  greedy_vector<indri::index::TermData*> _seenTerms;

  // ---- Cache structures -------------------------
  HashTable<int, indri::index::TermData*>* _termDataTable; /// in memory storage of data relating to terms -- partial inverted lists and statistics
  HashTable<const char*, term_cache_entry*>* _cache;

  ReadBuffer* _documentStatisticsBuffer; // read buffer for document stats
  ReadBuffer* _documentLengthBuffer;     // read buffer for document length

  // ---- Memory/cache management parameters -------
  size_t _listsSize; // memory for use by inverted list buffers
  size_t _memorySize; // upper bound for memory use
  size_t _termDataSize; // memory bound for termdata
  size_t _termCacheSize; // memory bound for termcache
  size_t _statisticsBufferSize; // memory bound for _documentStatisticsBuffer
  size_t _lengthBufferSize; // memory bound for _documentLengthBuffer
  float _queryProportion; // proportion of load assumed to be queries
  bool _batchBuild;

  INT64 _estimatePoint;  /// number of terms in the index when we should next check on flushing the inverted lists
  INT64 _lastCacheFlush; /// number of terms in the index when the cache was flushed last
};


#endif // INDRI_INDEX_HPP

