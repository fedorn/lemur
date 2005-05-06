/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#include "InvFPIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * tnt 04/2001 - created
 * dmf 10/22/2002 -- Add reading of compressed TermInfoLists.
 *
 *========================================================================*/

lemur::index::InvFPIndex::InvFPIndex() : InvIndex() {
}

lemur::index::InvFPIndex::InvFPIndex(const string &indexName) : InvIndex(indexName) {
}

lemur::index::InvFPIndex::~InvFPIndex() {
  // base class will take care of everything
}

lemur::api::DocInfoList* lemur::index::InvFPIndex::docInfoList(lemur::api::TERMID_T termID) const{
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docInfoList for invalid termID" << endl;    
    return NULL;
  }

  if (termID == 0)
    return NULL;
  ifstream *indexin = &(invfstreams[lookup[termID].fileid]);
  indexin->seekg(lookup[termID].offset, ios::beg);
  lemur::api::DocInfoList* doclist;
  InvFPDocList* dlist = new InvFPDocList();
  bool success;

  if (!names[VERSION_NUM].empty()) {
    // version 1.9 is compressed and must be decompressed
    success = dlist->binReadC(*indexin);
  } else {
    success = dlist->binRead(*indexin);
  }

  if (!success) {
    return NULL;
  } else {
    doclist = dlist;
    return doclist;
  }
}

lemur::api::TermInfoList* lemur::index::InvFPIndex::termInfoList(lemur::api::DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream <<  "Error trying to get termInfoList for invalid docID.\n" << endl;
    return NULL;
  }
    
  if (docID == 0)
    return NULL;
  bool success;
  ifstream *indexin = &(dtfstreams[dtlookup[docID].fileid]);
  indexin->seekg(dtlookup[docID].offset, ios::beg);
  lemur::api::TermInfoList* termlist;
  InvFPTermList* tlist = new InvFPTermList();
  if (names[VERSION_NUM] >= "2.0.1")
    // version 2.0 is compressed and must be decompressed
    success = tlist->binReadC(*indexin);
  else
    success = tlist->binRead(*indexin);

  if (!success) {
    return NULL;
  } else {
    //we have to count the terms to make it bag of words from sequence
    tlist->countTerms();
  }
  termlist = tlist;
  return termlist;
}

lemur::api::TermInfoList* lemur::index::InvFPIndex::termInfoListSeq(lemur::api::DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get termInfoList for invalid docID.\n"<< endl;
    return NULL;
  }
    
  if (docID == 0)
    return NULL;
  bool success;
  ifstream *indexin = &(dtfstreams[dtlookup[docID].fileid]);
  indexin->seekg(dtlookup[docID].offset, ios::beg);
  lemur::api::TermInfoList* termlist;
  InvFPTermList* tlist = new InvFPTermList();
  if (names[VERSION_NUM] >= "2.0.1")
    // version 2.0 is compressed and must be decompressed
    success = tlist->binReadC(*indexin);
  else
    success = tlist->binRead(*indexin);

  if (!success) {
    return NULL;
  }
  termlist = tlist;
  return termlist;
}

lemur::api::COUNT_T lemur::index::InvFPIndex::docLengthCounted(lemur::api::DOCID_T docID)  const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get docLengthCounted for invalid docID."<< endl;
    return 0;
  }
  // this is OOV
  if (docID == 0)
    return 0;

  InvFPTermList* tl = (InvFPTermList*) this->termInfoListSeq(docID);
  // if this fails for whatever reason
  if (!tl)
    return 0;

  lemur::api::COUNT_T count = tl->termCount();
  delete tl;
  return count;
}

