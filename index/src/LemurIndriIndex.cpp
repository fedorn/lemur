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



/*
 * NAME DATE - COMMENTS
 * dmf 09/04 - Lemur Index API wrapper on Indri Repository
 */
#include "LemurIndriIndex.hpp"
#include "IndriDocMgr.hpp"
#include "indri/IndriIndex.hpp"

/*! Lemur Index API wrapper for Indri Repository.
 */
LemurIndriIndex::LemurIndriIndex() : _docMgr(NULL) {
  _repository = new Repository();
}
 
LemurIndriIndex::~LemurIndriIndex() {
  close();
  delete(_repository);
  delete(_docMgr);
}

bool LemurIndriIndex::open(const std::string& indexName) {
  // parse pathname out of indexname.
  // /index/index.ind == 16 chars
  int len = indexName.length() - 16;
  _repositoryName = indexName.substr(0, len);
  _repository->openRead( _repositoryName, NULL );
  _docMgr = new IndriDocMgr(*_repository, _repositoryName);
  return true;
}

void LemurIndriIndex::close() {
  _repository->close();
}

TERMID_T LemurIndriIndex::term(const TERM_T &word) const { 
  return _repository->index()->term(word); 
}

const TERM_T LemurIndriIndex::term(TERMID_T termID) const { 
  return _repository->index()->term(termID); 
}

DOCID_T LemurIndriIndex::document(const EXDOCID_T &docIDStr) const { 
  return _repository->index()->document(docIDStr); 
};

const EXDOCID_T LemurIndriIndex::document(DOCID_T docID) const { 
  return _repository->index()->document(docID); 
}

COUNT_T LemurIndriIndex::docCount() const { 
  return _repository->index()->docCount(); 
}

COUNT_T LemurIndriIndex::termCountUnique() const { 
  return _repository->index()->termCountUnique(); 
}

COUNT_T LemurIndriIndex::termCount(TERMID_T termID) const { 
  return _repository->index()->termCount(termID);
}

COUNT_T LemurIndriIndex::termCount() const { 
  return _repository->index()->termCount(); 
}

float LemurIndriIndex::docLengthAvg() const { 
  return _repository->index()->docLengthAvg(); 
}

COUNT_T LemurIndriIndex::docCount(TERMID_T termID) const { 
  return _repository->index()->docCount(termID); 
}

COUNT_T LemurIndriIndex::docLength( DOCID_T documentID ) const { 
  return _repository->index()->docLength(documentID); 
}

DocInfoList* LemurIndriIndex::docInfoList(TERMID_T termID) const { 
  return _repository->index()->docInfoList(termID); 
}

TermInfoList* LemurIndriIndex::termInfoList(DOCID_T docID) const { 
  return _repository->index()->termInfoList(docID); 
}

TermInfoList* LemurIndriIndex::termInfoListSeq(DOCID_T docID) const { 
  return _repository->index()->termInfoListSeq(docID); 
}

