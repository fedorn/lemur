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

namespace lemur
{
  namespace index 
  {
    
    /*! Lemur Index API wrapper for Indri Repository.
     */
    class LemurIndriIndex : public lemur::api::Index {
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
      lemur::api::TERMID_T term(const lemur::api::TERM_T &word) const;

      /// Convert a termID to its spelling
      const lemur::api::TERM_T term(lemur::api::TERMID_T termID) const;

      /// Convert a spelling to docID
      lemur::api::DOCID_T document(const lemur::api::EXDOCID_T &docIDStr) const;

      /// Convert a docID to its spelling
      const lemur::api::EXDOCID_T document(lemur::api::DOCID_T docID) const;

      //@}

      /// @name Summary counts
      //@{

      /// Total count (i.e., number) of documents in collection
      lemur::api::COUNT_T docCount() const;

      /// Total count of unique terms in collection
      lemur::api::COUNT_T termCountUnique() const;

      /// Total counts of a term in collection
      lemur::api::COUNT_T termCount(lemur::api::TERMID_T termID) const;

      /// Total counts of all terms in collection
      lemur::api::COUNT_T termCount() const;

      /// Average document length 
      float docLengthAvg() const;

      /// Total counts of doc with a given term
      lemur::api::COUNT_T docCount(lemur::api::TERMID_T termID) const;

      /// return length of the document
      lemur::api::COUNT_T docLength( lemur::api::DOCID_T documentID ) const;

      //@}

      /// doc entries in a term index with positions, @see DocList
      lemur::api::DocInfoList* docInfoList(lemur::api::TERMID_T termID) const;

      /// word entries in a document index (bag of words), @see TermList
      lemur::api::TermInfoList* termInfoList(lemur::api::DOCID_T docID) const;

      /// word entries in a document index (sequence of words), @see TermList
      lemur::api::TermInfoList* termInfoListSeq(lemur::api::DOCID_T docID) const;

      const lemur::api::DocumentManager* docManager(lemur::api::DOCID_T docID) const {
        return _docMgr;
      }
  
      const string &getRepositoryName() const {return _repositoryName;}
  
    private:
      indri::index::Index* _indexWithDocument( indri::collection::Repository::index_state& indexes, lemur::api::DOCID_T documentID ) const;
      indri::collection::Repository *_repository;
      string _repositoryName;
      lemur::api::DocumentManager *_docMgr;
    };
  }
}

#endif // LEMURINDRI_INDEX_HPP

