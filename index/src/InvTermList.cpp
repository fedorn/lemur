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


#include "InvTermList.hpp"

InvTermList::InvTermList() {
  length = 0;
  iter = NULL;
  listlen = 0;
  uid = -1;
  list = NULL;
  end = NULL;
}

InvTermList::~InvTermList() {
  delete[](list);
}

void InvTermList::startIteration() const{
  iter = list;
}

bool InvTermList::hasMore() const{
  return iter != end;
}

TermInfo* InvTermList::nextEntry() const{
  //  static InvFPTerm info;

  entry.termID(*iter);
  iter++;
  entry.count(*iter);
  iter++;

  return &entry;
}

/// set element from position, returns pointer to the element
TermInfo* InvTermList::getElement(TermInfo* elem, POS_T position) const {
  // info is stored in LOC_T* as docid freq .. ..
  LOC_T* ip = (LOC_T*) position;
  elem->termID(*ip);
  ip++;
  elem->count(*ip);
  return elem;
}
/// advance position
POS_T InvTermList::nextPosition(POS_T position) const {
  // info is stored in LOC_T* as docid freq .. ..
  return (POS_T) (((LOC_T*) position) + 2);
}

bool InvTermList::binRead(ifstream& infile){
  if (infile.eof())
    return false;

  infile.read((char*) &uid, sizeof(DOCID_T));
  if (!(infile.gcount() == sizeof(DOCID_T)))
    return false;

  infile.read((char*) &length, sizeof(COUNT_T));
  if (!(infile.gcount() == sizeof(COUNT_T)))
    return false;

  infile.read((char*) &listlen, sizeof(COUNT_T));
  if (!(infile.gcount() == sizeof(COUNT_T)))
    return false;

  //  if (list)
  //    free(list);
  delete[](list);
  //  list = (int*) malloc(sizeof(int)*listlen);
  // use new/delete[] so an exception will be thrown if out of memory.
  list = new LOC_T[listlen];
  infile.read((char*) list, sizeof(LOC_T) * listlen);
  if (!(infile.gcount() == (sizeof(LOC_T) * listlen))) {
    //    free(list);
    delete[](list);
    list = NULL;
    end = NULL;
    return false;
  }
  end = list+listlen;
  return true;
}


