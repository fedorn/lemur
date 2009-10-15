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
#include "IndriFieldInfoList.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/ScopedLock.hpp"
#include "IndriDocMgr.hpp"
#include "InvFPTermList.hpp"
#include "indri/Index.hpp"
#include "indri/Path.hpp"

lemur::index::LemurIndriIndex::LemurIndriIndex() : _docMgr(NULL) {
  _repository = new indri::collection::Repository();
}
 
lemur::index::LemurIndriIndex::~LemurIndriIndex() {
  close();
  delete(_repository);
  delete(_docMgr);
}

bool lemur::index::LemurIndriIndex::open(const std::string& indexName) {
  // Have to remove the trailing ".ind" from the name.
  //
  _repositoryName = indexName; //indri::file::Path::basename(indexName);
  _repository->openRead( _repositoryName, NULL );
  _docMgr = new lemur::parse::IndriDocMgr(*_repository, _repositoryName);
  return true;
}

void lemur::index::LemurIndriIndex::close() {
  _repository->close();
}

lemur::api::TERMID_T lemur::index::LemurIndriIndex::term(const lemur::api::TERM_T &word) const { 
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

const lemur::api::TERM_T lemur::index::LemurIndriIndex::term(lemur::api::TERMID_T termID) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = (*indexes)[0];
  indri::thread::ScopedLock lock( index->statisticsLock() );
  return index->term( termID );
}

lemur::api::DOCID_T lemur::index::LemurIndriIndex::document(const lemur::api::EXDOCID_T &docIDStr) const { 
  std::vector<lemur::api::DOCID_T> ids = _repository->collection()->retrieveIDByMetadatum("docno", docIDStr);
  lemur::api::DOCID_T docid = 0;
  if (ids.size() > 0) 
    docid = ids[0];
  return docid;
}

const lemur::api::EXDOCID_T lemur::index::LemurIndriIndex::document(lemur::api::DOCID_T docID) const { 
  return _repository->collection()->retrieveMetadatum(docID, "docno");
}

lemur::api::COUNT_T lemur::index::LemurIndriIndex::docCount() const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  lemur::api::COUNT_T total = 0;
  
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->documentCount();
  }
  return total;
}

lemur::api::COUNT_T lemur::index::LemurIndriIndex::termCountUnique() const { 
  return _repository->indexes()->back()->uniqueTermCount(); 
}

lemur::api::COUNT_T lemur::index::LemurIndriIndex::termCount(lemur::api::TERMID_T termID) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  lemur::api::COUNT_T total = 0;
  std::string word(term(termID));
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->termCount( word );
  }

  return total;
}

lemur::api::COUNT_T lemur::index::LemurIndriIndex::termCount() const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  lemur::api::COUNT_T total = 0;

  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->termCount();
  }

  return total;
}

float lemur::index::LemurIndriIndex::docLengthAvg() const { 
  return (float)termCount()/(float)docCount(); 
}

lemur::api::COUNT_T lemur::index::LemurIndriIndex::docCount(lemur::api::TERMID_T termID) const { 
  std::string word(term(termID));
  indri::collection::Repository::index_state indexes = _repository->indexes();
  lemur::api::COUNT_T total = 0;
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    total += (*indexes)[i]->documentCount( word );
  }
  return total;
}

lemur::api::COUNT_T lemur::index::LemurIndriIndex::docLength( lemur::api::DOCID_T documentID ) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = _indexWithDocument( indexes, documentID );

  if( index ) {
    indri::thread::ScopedLock lock( index->statisticsLock() );
    return index->documentLength( documentID );
  }
  return 0;
}

lemur::api::DocInfoList* lemur::index::LemurIndriIndex::docInfoList(lemur::api::TERMID_T termID) const { 
  if (termID == 0) return NULL; //OOV
  // make a DocInfoList to copy the data into.
  // string length of term does not matter.
  InvFPDocList *newList = new InvFPDocList(termID, 0);
  indri::collection::Repository::index_state indexes = _repository->indexes();
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    indri::index::DocListIterator *dlist = (*indexes)[i]->docListIterator(termID);
    if (dlist == NULL) continue; // skip this index
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

lemur::api::TermInfoList* lemur::index::LemurIndriIndex::termInfoList(lemur::api::DOCID_T docID) const { 
  lemur::index::InvFPTermList *list = (lemur::index::InvFPTermList *)termInfoListSeq(docID);
  list->countTerms();
  return list;
}

lemur::api::TermInfoList* lemur::index::LemurIndriIndex::termInfoListSeq(lemur::api::DOCID_T docID) const { 
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = _indexWithDocument( indexes, docID );
  lemur::api::TermInfoList *list = NULL;
  if( index ) {
    indri::thread::ScopedLock lock( index->statisticsLock() );
    const indri::index::TermList* termList = index->termList( docID );
    indri::utility::greedy_vector<lemur::api::TERMID_T> termIDs = termList->terms();
    std::vector<lemur::index::LocatedTerm> locs;
    
    // indri indexes stopwords as [OOV], so skip those
    for( int i = 0; i < termIDs.size(); i++) {
      if (termIDs[i] != 0) {
        lemur::index::LocatedTerm lt;
        lt.term = termIDs[i];
        lt.loc = i;
        locs.push_back(lt);
      }
    }
    list = new lemur::index::InvFPTermList(docID, locs.size(), locs);
    delete(termList);
  }
  return list;
}

indri::index::Index* lemur::index::LemurIndriIndex::_indexWithDocument( indri::collection::Repository::index_state& indexes, lemur::api::DOCID_T documentID ) const {
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

// Convert a field name to a field ID (for those index types that support fields)
const int lemur::index::LemurIndriIndex::field(std::string fieldName) const {
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index *thisIndex=(*indexes)[0];
  return thisIndex->field(fieldName);
}

// Convert a field name to a field ID (for those index types that support fields)
const int lemur::index::LemurIndriIndex::field(const char *fieldName) const {
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index *thisIndex=(*indexes)[0];
  return thisIndex->field(fieldName);
}

// Convert a field ID to a field name (for those index types that support fields)
const std::string lemur::index::LemurIndriIndex::field(int fieldID) const {
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index *thisIndex=(*indexes)[0];
  return thisIndex->field(fieldID);
}

// returns a new instance of FieldInfoList which represents all field entities in a document index, you must delete the instance later. @see FieldInfoList 
// Note that not all index types support fields - those that do should override this method.
lemur::api::FieldInfoList *lemur::index::LemurIndriIndex::fieldInfoList(lemur::api::DOCID_T docID) const {
  // get the index for this document
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = _indexWithDocument( indexes, docID );

  // ensure we do have an index (i.e. if the docID was invalid...)
  if (!index) return NULL;
  
  // and the indri term-list
  const indri::index::TermList *tList=index->termList((int)docID);

  // ensure we have a term list!
  if (!tList) return NULL;

  // create our field info list object
  lemur::api::IndriFieldInfoList *retVal=new lemur::api::IndriFieldInfoList(tList->fields());
  delete tList;
  // and return
  return retVal;
}

// returns a new instance of FieldInfoList which represents field entities in a document index for a specific field, you must delete the instance later. @see FieldInfoList 
// Note that not all index types support fields - those that do should override this method.
lemur::api::FieldInfoList *lemur::index::LemurIndriIndex::fieldInfoList(lemur::api::DOCID_T docID, int fieldID) const {
  // get the index for this document
  indri::collection::Repository::index_state indexes = _repository->indexes();
  indri::index::Index* index = _indexWithDocument( indexes, docID );

  // ensure we do have an index (i.e. if the docID was invalid...)
  if (!index) return NULL;
  
  // and the indri term-list
  const indri::index::TermList *tList=index->termList((int)docID);

  // ensure we have a term list!
  if (!tList) return NULL;

  // create a blank field info list object
  lemur::api::IndriFieldInfoList *retVal=new lemur::api::IndriFieldInfoList();

  // loop through our fields and insert those that match the field ID
  const indri::utility::greedy_vector< indri::index::FieldExtent > fieldVec=tList->fields();
  int numFields=fieldVec.size();
  for (int i=0; i < numFields; i++) {
    indri::index::FieldExtent thisField=fieldVec[i];
    if (thisField.id==fieldID) {
      retVal->add(thisField);
    }
  }
  delete tList;
  // and return
  return retVal;
}


// Fetch the named metadata attribute for a list of document ids.
std::vector<std::string> lemur::index::LemurIndriIndex::documentMetadata(const std::vector< lemur::api::DOCID_T > &documentIDs, const std::string &attributeName) {
  std::vector<std::string> retVec;
  retVec.reserve(documentIDs.size());
  retVec.clear();

  indri::collection::CompressedCollection* collection = _repository->collection();

  std::vector< lemur::api::DOCID_T >::const_iterator vIter=documentIDs.begin();
  while (vIter!=documentIDs.end() && collection) {
    retVec.push_back(collection->retrieveMetadatum( (*vIter), attributeName ) );
  }

  return retVec;
}

// Fetch the named metadata attribute for a single document id.
std::vector<std::string> lemur::index::LemurIndriIndex::documentMetadata(lemur::api::DOCID_T documentID, const std::string &attributeName) {
  std::vector<std::string> retVec;
  retVec.clear();
  indri::collection::CompressedCollection* collection = _repository->collection();
  retVec.push_back(collection->retrieveMetadatum( documentID, attributeName ));
  return retVec;
}


