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
  if (list != NULL)
    free(list);
}

void InvTermList::startIteration(){
  iter = list;
}

bool InvTermList::hasMore(){
  return iter != end;
}

TermInfo* InvTermList::nextEntry(){
  static InvFPTerm info;

  info.tid = *iter;
  iter++;
  info.freq = *iter;
  iter++;

  return &info;
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

  if (list)
    free(list);

  list = (int*) malloc(sizeof(int)*listlen);
  infile.read((char*) list, sizeof(int) * listlen);
  if (!(infile.gcount() == (sizeof(int) * listlen))) {
    free(list);
    list = NULL;
    end = NULL;
    return false;
  }
  end = list+listlen;
  return true;
}


