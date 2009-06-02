/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


/* 
 * dmf 09/2004 - created (minimalist docmgr API for indri indexer)
 */

#include "IndriDocMgr.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/ScopedLock.hpp"
//
// _indexWithDocument
//

indri::index::Index* lemur::parse::IndriDocMgr::_indexWithDocument( indri::collection::Repository::index_state& indexes, lemur::api::DOCID_T documentID ) const {
  for( int i=0; i<indexes->size(); i++ ) {
    indri::thread::ScopedLock lock( (*indexes)[i]->statisticsLock() );
    lemur::api::DOCID_T lowerBound = (*indexes)[i]->documentBase();
    lemur::api::DOCID_T upperBound = (*indexes)[i]->documentBase() + (*indexes)[i]->documentCount();
    
    if( lowerBound <= documentID && upperBound > documentID ) {
      return (*indexes)[i];
    }
  }
  return 0;
}

// caller delete[]s
char *lemur::parse::IndriDocMgr::getDoc(const lemur::api::EXDOCID_T &docID) const {
  std::vector<lemur::api::DOCID_T> ids = _repository.collection()->retrieveIDByMetadatum("docno", docID);
  lemur::api::DOCID_T docid = ids[0];
  indri::api::ParsedDocument *doc = _repository.collection()->retrieve(docid);
  const char *txt = doc->text;
  char *retval = new char[strlen(txt) + 1];
  strcpy(retval, txt);
  delete(doc);
  return retval;
}


vector<lemur::parse::Match> lemur::parse::IndriDocMgr::getOffsets(const lemur::api::EXDOCID_T &docID) const {
  vector<Match> offsets;
  std::vector<lemur::api::DOCID_T> ids = _repository.collection()->retrieveIDByMetadatum("docno", docID);
  lemur::api::DOCID_T docid = ids[0];
  indri::api::ParsedDocument *doc = _repository.collection()->retrieve(docid);
  indri::collection::Repository::index_state indexes = _repository.indexes();
  indri::index::Index* index = _indexWithDocument( indexes, docid );
  if (index) {
    indri::thread::ScopedLock lock( index->statisticsLock() );
    const indri::index::TermList* termList = index->termList( docid );
    const indri::utility::greedy_vector<lemur::api::TERMID_T>& terms = termList->terms();
    indri::utility::greedy_vector<indri::parse::TermExtent> pos = doc->positions;  
    unsigned int len = pos.size();
    offsets.resize(len);
    for (int i = 0; i < len; i++) {
      offsets[i].start = pos[i].begin;
      offsets[i].end = pos[i].end;
      offsets[i].pos = i;
      offsets[i].termid = terms[i];
    }
    delete(termList);
  }
  delete(doc);  
  return offsets;
}
