/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  author: fff, dmf
*/
#include "ProxInfo.hpp"
#include "Exception.hpp"

ProxInfo::ProxInfo(int num, int tf, DocInfoList *dl) : nextPos(0), 
						       posList(NULL),  
						       size(0),
						       listSize(tf) { 
  dList = dynamic_cast<InvFPDocList *>(dl);
  if (dList == NULL) {
    throw Exception("ProxInfo", "InvFPDocList required from index");
  }
}

ProxInfo::ProxInfo(int num, int tf, int *pl) : nextPos(0), 
					       posList(pl),  
					       size(0), 
					       dList(NULL),
					       listSize(tf) { 
  //construct an InvFPDocList.
  //InvFPDocList(int id, int listlen, int* list, int fr, int* ldocid, int len)
  // id --  termID (can be bogus value, use 0)
  // listlen -- 2 * num entries + tf
  // list -- pl
  // fr -- df -- can be ignored, use 0
  // ldocId -- lastid can be pl
  // len -- strlength of term(termID) -- can be ignored, use 0.
  dList = new InvFPDocList(0, (2*num + tf), pl, 0, pl, 0);
}

bool ProxInfo::nextDoc() {
  if (dList->hasMore()) {
    currEntry = *(InvFPDocInfo *)dList->nextEntry();
    return true;
  } else {
    return false;
  }
}


bool ProxInfo::nextDoc(int did) {
  if (!dList) return false;
  while (did > id() && nextDoc()) {
  }
  return (did == id());
}

int ProxInfo::posSize() {
  return listSize;
}
