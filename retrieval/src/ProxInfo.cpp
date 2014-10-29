/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
  author: fff, dmf
*/
#include "ProxInfo.hpp"
#include "Exception.hpp"
using namespace lemur::api;

lemur::retrieval::ProxInfo::ProxInfo(int num, int tf, const DocInfoList *dl) : 
  nextPos(0), posList(NULL),  size(0), listSize(tf) { 
  dList = dynamic_cast<const lemur::index::InvFPDocList *>(dl);
  if (dList == NULL) {
    LOC_T *posList = new LOC_T[2*num + tf];
    LOC_T *tmpList = posList;
    for (DocInfoList::iterator iter = dl->begin(); iter != dl->end(); iter++){
      DocInfo &info = *iter;
      DOCID_T did = info.docID();
      COUNT_T cnt = info.termCount();
      *tmpList++ = did;
      *tmpList++ = cnt;
      for (int i = 0; i < cnt; i++) *tmpList++ = 0; 
    }
    dList = new lemur::index::InvFPDocList(0, (2*num + tf), posList, 0, posList, 0);
    delete(dl);
  }
}

lemur::retrieval::ProxInfo::ProxInfo(int num, int tf, LOC_T *pl) : 
  nextPos(0), posList(pl), size(0), dList(NULL), listSize(tf) { 
  //construct an InvFPDocList.
  //InvFPDocList(int id, int listlen, int* list, int fr, int* ldocid, int len)
  // id --  termID (can be bogus value, use 0)
  // listlen -- 2 * num entries + tf
  // list -- pl
  // fr -- df -- can be ignored, use 0
  // ldocId -- lastid can be pl
  // len -- strlength of term(termID) -- can be ignored, use 0.
  dList = new lemur::index::InvFPDocList(0, (2*num + tf), pl, 0, pl, 0);
}

bool lemur::retrieval::ProxInfo::nextDoc() {
  if (dList->hasMore()) {
    currEntry = *(lemur::index::InvFPDocInfo *)dList->nextEntry();
    return true;
  } else {
    return false;
  }
}


bool lemur::retrieval::ProxInfo::nextDoc(DOCID_T did) {
  if (!dList) return false;
  while (did > id() && nextDoc()) {
  }
  return (did == id());
}

int lemur::retrieval::ProxInfo::posSize() const{
  return listSize;
}
