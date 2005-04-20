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



#ifndef LEMURINDRI_INDEX_HPP
#define LEMURINDRI_INDEX_HPP

/*
 * NAME DATE - COMMENTS
 * dmf 09/04 - Lemur Index API wrapper on Indri Repository
*/

#include "Index.hpp"
#include "indri/Repository.hpp"

/*! Lemur Index API wrapper for Indri Repository.
 */
class LemurIndriIndex : public Index {
public:
  /// Create.
  LemurIndriIndex();

  virtual ~LemurIndriIndex();

  /// @name Open index 
  //@{

  /// Open previously created Repository with given prefix
  bool open(const std::string& indexName);

  /// Open previously created Repository with given prefix
  bool open( const char* indexName ) {
    return open(string(indexName));
  }
  /// close the Repository
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

  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  COUNT_T docCount() const;

  /// Total count of unique terms in collection
  COUNT_T termCountUnique() const;

  /// Total counts of a term in collection
  COUNT_T termCount(TERMID_T termID) const;

  /// Total counts of all terms in collection
  COUNT_T termCount() const;

  /// Average document length 
  float docLengthAvg() const;

  /// Total counts of doc with a given term
  COUNT_T docCount(TERMID_T termID) const;

  /// return length of the document
  COUNT_T docLength( DOCID_T documentID ) const;

  //@}

  /// doc entries in a term index with positions, @see DocList
  DocInfoList* docInfoList(TERMID_T termID) const;

  /// word entries in a document index (bag of words), @see TermList
  TermInfoList* termInfoList(DOCID_T docID) const;

  /// word entries in a document index (sequence of words), @see TermList
  TermInfoList* termInfoListSeq(DOCID_T docID) const;

  const DocumentManager* docManager(DOCID_T docID) const {
    return _docMgr;
  }
  
  const string &getRepositoryName() const {return _repositoryName;}
  
private:
  indri::index::Index* _indexWithDocument( indri::collection::Repository::index_state& indexes, int documentID ) const;
  indri::collection::Repository *_repository;
  string _repositoryName;
  DocumentManager *_docMgr;
};

#endif // LEMURINDRI_INDEX_HPP

