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
  
  //  const Repository *getRepository() const {return _repository;}
  const string &getRepositoryName() const {return _repositoryName;}
  
private:
  Repository *_repository;
  string _repositoryName;
  DocumentManager *_docMgr;
};

#endif // LEMURINDRI_INDEX_HPP

