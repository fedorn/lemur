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
#include "indri/IndriIndex.hpp"
#include "indri/IndriTermInfoList.hpp"
#include "indri/CompressedCollection.hpp"

// caller delete[]s
char *IndriDocMgr::getDoc(const EXDOCID_T &docID) const {
  DOCID_T docid = _repository.index()->document(docID);
  ParsedDocument *doc = _repository.collection()->retrieve(docid);
  const char *txt = doc->text;
  char *retval = new char[strlen(txt) + 1];
  strcpy(retval, txt);
  delete(doc);
  return retval;
}

vector<Match> IndriDocMgr::getOffsets(const EXDOCID_T &docID) const {
  vector<Match> offsets;
  DOCID_T docid = _repository.index()->document(docID);
  ParsedDocument *doc = _repository.collection()->retrieve(docid);
  // list of term ids to derive positions
  TermInfoList *dt = _repository.index()->termInfoListSeq(docid);
  TermInfoList::iterator iter = dt->begin();
  
  greedy_vector<TermExtent> pos = doc->positions;  
  unsigned int len = pos.size();
  offsets.resize(len);
  TermInfo term;
  for (int i = 0; i < len; i++) {
    offsets[i].start = pos[i].begin;
    offsets[i].end = pos[i].end;
    offsets[i].pos = i;
    iter.seek((void *)i);
    offsets[i].termid = (*iter).termID();
  }
  delete(doc);
  delete(dt);
  return offsets;
}
