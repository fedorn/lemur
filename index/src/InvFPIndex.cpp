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


#include "InvFPIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * tnt 04/2001 - created
 * dmf 10/22/2002 -- Add reading of compressed TermInfoLists.
 *
 *========================================================================*/

InvFPIndex::InvFPIndex() : InvIndex() {
}

InvFPIndex::InvFPIndex(const char* indexName) : InvIndex(indexName) {
}

InvFPIndex::~InvFPIndex() {
  // base class will take care of everything
}

DocInfoList* InvFPIndex::docInfoList(int termID){
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docInfoList for invalid termID" << endl;    
    return NULL;
  }

  if (termID == 0)
    return NULL;
  ifstream *indexin = &(invfstreams[lookup[termID].fileid]);
  indexin->seekg(lookup[termID].offset, ios::beg);
  DocInfoList* doclist;
  InvFPDocList* dlist = new InvFPDocList();
  bool success;

  if (strcmp(names[VERSION_NUM], "")) {
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

TermInfoList* InvFPIndex::termInfoList(int docID){
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream <<  "Error trying to get termInfoList for invalid docID.\n" << endl;
    return NULL;
  }
    
  if (docID == 0)
    return NULL;
  bool success;
  ifstream *indexin = &(dtfstreams[dtlookup[docID].fileid]);
  indexin->seekg(dtlookup[docID].offset, ios::beg);
  TermInfoList* termlist;
  InvFPTermList* tlist = new InvFPTermList();
  if (strcmp(names[VERSION_NUM], "2.0.1") >= 0 )
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

TermInfoList* InvFPIndex::termInfoListSeq(int docID){
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get termInfoList for invalid docID.\n"<< endl;
    return NULL;
  }
    
  if (docID == 0)
    return NULL;
  bool success;
  ifstream *indexin = &(dtfstreams[dtlookup[docID].fileid]);
  indexin->seekg(dtlookup[docID].offset, ios::beg);
  TermInfoList* termlist;
  InvFPTermList* tlist = new InvFPTermList();
  if (strcmp(names[VERSION_NUM], "2.0.1") >= 0)
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



