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
    *lemurstream << "Error:  Trying to get docInfoList for invalid termID" << endl;    
    return NULL;
  }

  if (termID == 0)
    return NULL;

  ifstream indexin;

  indexin.open(invfiles[lookup[termID].fileid], ios::in | ios::binary);
  indexin.seekg(lookup[termID].offset, ios::beg);
  DocInfoList* doclist;
  InvFPDocList* dlist = new InvFPDocList();
  if (!dlist->binRead(indexin)) {
    indexin.close();
    return NULL;
  } else {
    indexin.close();
    doclist = dlist;
    return doclist;
  }
}

TermInfoList* InvFPIndex::termInfoList(int docID){
  if ((docID < 0) || (docID > counts[DOCS])) {
    *lemurstream <<  "Error trying to get termInfoList for invalid docID.\n" << endl;
    return NULL;
  }
    
  if (docID == 0)
    return NULL;

  ifstream indexin;
  indexin.open(dtfiles[dtlookup[docID].fileid], ios::in | ios::binary);
  indexin.seekg(dtlookup[docID].offset, ios::beg);
  TermInfoList* termlist;
  InvFPTermList* tlist = new InvFPTermList();
  if (!tlist->binRead(indexin)) {
    indexin.close();
    return NULL;
  } else {
    //we have to count the terms to make it bag of words from sequence
    tlist->countTerms();
  }

  indexin.close();
  termlist = tlist;
  return termlist;
}

TermInfoList* InvFPIndex::termInfoListSeq(int docID){
  if ((docID < 0) || (docID > counts[DOCS])) {
    *lemurstream << "Error trying to get termInfoList for invalid docID.\n"<< endl;
    return NULL;
  }
    
  if (docID == 0)
    return NULL;

  ifstream indexin;
  indexin.open(dtfiles[dtlookup[docID].fileid], ios::in | ios::binary);
  indexin.seekg(dtlookup[docID].offset, ios::beg);
  TermInfoList* termlist;
  InvFPTermList* tlist = new InvFPTermList();
  if (!tlist->binRead(indexin)) {
    indexin.close();
    return NULL;
  }

  indexin.close();
  termlist = tlist;
  return termlist;
}



