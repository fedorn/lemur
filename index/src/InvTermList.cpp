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

  entry.tid = *iter;
  iter++;
  entry.freq = *iter;
  iter++;

  return &entry;
}

bool InvTermList::binRead(ifstream& infile){
  if (infile.eof())
    return false;

  infile.read((char*) &uid, sizeof(DOCID_T));
  if (!(infile.gcount() == sizeof(DOCID_T)))
    return false;

  infile.read((char*) &length, sizeof(int));
  if (!(infile.gcount() == sizeof(int)))
    return false;

  infile.read((char*) &listlen, sizeof(int));
  if (!(infile.gcount() == sizeof(int)))
    return false;

  //  if (list)
  //    free(list);
  delete[](list);
  //  list = (int*) malloc(sizeof(int)*listlen);
  // use new/delete[] so an exception will be thrown if out of memory.
  list = new int[listlen];
  infile.read((char*) list, sizeof(int) * listlen);
  if (!(infile.gcount() == (sizeof(int) * listlen))) {
    //    free(list);
    delete[](list);
    list = NULL;
    end = NULL;
    return false;
  }
  end = list+listlen;
  return true;
}


