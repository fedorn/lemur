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

/*
 * NAME DATE - COMMENTS
 * dmf 09/04 - Lemur Index API wrapper on Indri Repository
 */
#include "LemurIndriIndex.hpp"
#include "InvFPDocList.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/ScopedLock.hpp"
#include "IndriDocMgr.hpp"
#include "indri/IndriTermInfoList.hpp"
#include "indri/Index.hpp"
#include "indri/Path.hpp"

LemurIndriIndex::LemurIndriIndex() : _docMgr(NULL) {
  _repository = new indri::collection::Repository();
}
 
LemurIndriIndex::~LemurIndriIndex() {
  close();
  delete(_repository);
  delete(_docMgr);
}

bool LemurIndriIndex::open(const std::string& indexName) {
  // Have to remove the trailing ".ind" from the name.
  //
  std::string name = indri::file::Path::basename(indexName);
  _repository->openRead( name, NULL );
  _docMgr = new IndriDocMgr(*_repository, name);
  return true;
}

void LemurIndriIndex::close() {
  _repository->close();
}

TERMID_T LemurIndriIndex::term(const TERM_T &word) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = (*indexes)[0];
  std::string processed = _repository->processTerm( word );
  indri::thread::ScopedLock lock( index->statisticsLock() );
  if( processed.length() != 0 ) {
    return index->term( processed.c_str() );
  } else {
    return 0;
  }
}

const TERM_T LemurIndriIndex::term(TERMID_T termID) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = (*indexes)[0];
  indri::thread::ScopedLock lock( index->statisticsLock() );
  return index->term( termID );
}

DOCID_T LemurIndriIndex::document(const EXDOCID_T &docIDStr) const { 
  std::vector<int> ids = _repository->collection()->retrieveIDByMetadatum("docno", docIDStr);
  DOCID_T docid = ids[0];
  return docid;
};

const EXDOCID_T LemurIndriIndex::document(DOCID_T docID) const { 
  return _repository->collection()->retrieveMetadatum(docID, "docno");
}

COUNT_T LemurIndriIndex::docCount() const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  COUNT_T total = 0;
  
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->documentCount();
  }
  return total;
}

COUNT_T LemurIndriIndex::termCountUnique() const { 
  return _repository->indexes()->back()->uniqueTermCount(); 
}

COUNT_T LemurIndriIndex::termCount(TERMID_T termID) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  COUNT_T total = 0;
  std::string word(term(termID));
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->termCount( word );
  }

  return total;
}

COUNT_T LemurIndriIndex::termCount() const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  COUNT_T total = 0;

  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->termCount();
  }

  return total;
}

float LemurIndriIndex::docLengthAvg() const { 
  return (float)termCount()/(float)docCount(); 
}

COUNT_T LemurIndriIndex::docCount(TERMID_T termID) const { 
  std::string word(term(termID));
  indri::collection::Repository::index_state indexes = _repository->indexes();
  COUNT_T total = 0;
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->documentCount( word );
  }
  return total;
}

COUNT_T LemurIndriIndex::docLength( DOCID_T documentID ) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = _indexWithDocument( indexes, documentID );

  if( index ) {
    indri::thread::ScopedLock lock( index->statisticsLock() );
    return index->documentLength( documentID );
  }
  return 0;
}

DocInfoList* LemurIndriIndex::docInfoList(TERMID_T termID) const { 
  // make a DocInfoList to copy the data into.
  // string length of term does not matter.
  InvFPDocList *newList = new InvFPDocList(termID, 0);
  indri::collection::Repository::index_state indexes = _repository->indexes();
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    indri::index::DocListIterator *dlist = (*indexes)[i]->docListIterator(termID);
    dlist->startIteration();
    while (! dlist->finished()) {
      indri::index::DocListIterator::DocumentData* data = dlist->currentEntry();
      for (int j = 0; j < data->positions.size(); j++)
        newList->addLocation(data->document, data->positions[j]);
      dlist->nextEntry();
    }
    delete(dlist);
  }
  return newList;
}

TermInfoList* LemurIndriIndex::termInfoList(DOCID_T docID) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = _indexWithDocument( indexes, docID );
  TermInfoList *list = NULL;
  if( index ) {
    indri::thread::ScopedLock lock( index->statisticsLock() );
    const indri::index::TermList* termList = index->termList( docID );
    list = new indri::index::BagList(termList);
  }
  return list;
}

TermInfoList* LemurIndriIndex::termInfoListSeq(DOCID_T docID) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = _indexWithDocument( indexes, docID );
  TermInfoList *list = NULL;
  if( index ) {
    indri::thread::ScopedLock lock( index->statisticsLock() );
    const indri::index::TermList* termList = index->termList( docID );
    list = new indri::index::PositionList(termList);
  }
  return list;
}

indri::index::Index* LemurIndriIndex::_indexWithDocument( indri::collection::Repository::index_state& indexes, int documentID ) const {
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    int lowerBound = (*indexes)[i]->documentBase();
    int upperBound = (*indexes)[i]->documentBase() + (*indexes)[i]->documentCount();
    
    if( lowerBound <= documentID && upperBound > documentID ) {
      return (*indexes)[i];
    }
  }
  return 0;
}
