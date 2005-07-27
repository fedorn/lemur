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


#include "InvTermList.hpp"

lemur::index::InvTermList::InvTermList() {
  length = 0;
  iter = NULL;
  listlen = 0;
  uid = -1;
  list = NULL;
  end = NULL;
}

lemur::index::InvTermList::~InvTermList() {
  delete[](list);
}

void lemur::index::InvTermList::startIteration() const{
  iter = list;
}

bool lemur::index::InvTermList::hasMore() const{
  return iter != end;
}

lemur::api::TermInfo* lemur::index::InvTermList::nextEntry() const{
  //  static InvFPTerm info;

  entry.termID(*iter);
  iter++;
  entry.count(*iter);
  iter++;

  return &entry;
}

/// set element from position, returns pointer to the element
lemur::api::TermInfo* lemur::index::InvTermList::getElement(lemur::api::TermInfo* elem, lemur::api::POS_T position) const {
  // info is stored in LOC_T* as docid freq .. ..
  lemur::api::LOC_T* ip = list + position;
  elem->termID(*ip);
  ip++;
  elem->count(*ip);
  return elem;
}
/// advance position
lemur::api::POS_T lemur::index::InvTermList::nextPosition(lemur::api::POS_T position) const {
  // info is stored in LOC_T* as docid freq .. ..
  return (position + 2);
}

bool lemur::index::InvTermList::binRead(ifstream& infile){
  if (infile.eof())
    return false;

  infile.read((char*) &uid, sizeof(lemur::api::DOCID_T));
  if (!(infile.gcount() == sizeof(lemur::api::DOCID_T)))
    return false;

  infile.read((char*) &length, sizeof(lemur::api::COUNT_T));
  if (!(infile.gcount() == sizeof(lemur::api::COUNT_T)))
    return false;

  infile.read((char*) &listlen, sizeof(lemur::api::COUNT_T));
  if (!(infile.gcount() == sizeof(lemur::api::COUNT_T)))
    return false;

  //  if (list)
  //    free(list);
  delete[](list);
  //  list = (int*) malloc(sizeof(int)*listlen);
  // use new/delete[] so an exception will be thrown if out of memory.
  list = new lemur::api::LOC_T[listlen];
  infile.read((char*) list, sizeof(lemur::api::LOC_T) * listlen);
  if (!(infile.gcount() == (sizeof(lemur::api::LOC_T) * listlen))) {
    //    free(list);
    delete[](list);
    list = NULL;
    end = NULL;
    return false;
  }
  end = list+listlen;
  return true;
}


