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


#include "InvFPTermList.hpp"

InvFPTermList::InvFPTermList() {
  length = 0;
  index = 0;
  listlen = 0;
  uid = -1;
  list = NULL;
  counts = NULL;
}

InvFPTermList::~InvFPTermList() {
  if (list != NULL)
    delete[](list);
  if (counts != NULL)
    free(counts);
}

void InvFPTermList::startIteration(){
  index = 0;
}

bool InvFPTermList::hasMore(){
  return index != listlen;
}

TermInfo* InvFPTermList::nextEntry(){
//  TermInfo* tinfo;
  static InvFPTerm info;
  info.loc = list[index].loc;
  info.tid = list[index].term;
  if (counts)
    info.freq = counts[index];
  index++;
//  tinfo = info;
  return &info;
}

bool InvFPTermList::binRead(ifstream& infile){
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

  if (!(list == NULL))
    delete[](list);

  list = new LocatedTerm[listlen];
  infile.read((char*) list, sizeof(LocatedTerm) * listlen);
  if (!(infile.gcount() == (sizeof(LocatedTerm) * listlen))) {
    delete[](list);
    list = NULL;
    return false;
  }

  return true;
}

void InvFPTermList::countTerms(){
  // this is probably not the best way of doing this, but..
  // this is what happens when you adapt one implementation of something to match
  // something else that originally had other data structures.
  map<int, int> table;
  map<int, int>::iterator place;
  for (int i=0;i<listlen;i++) {
    place = table.find(list[i].term);
    if (place != table.end()) {
      (place->second)++;      
    } else {
      //store to table
      table[list[i].term]=1;
    }
  }

  listlen = table.size();
  LocatedTerm* listcopy = new LocatedTerm[listlen];
  counts = (int*) malloc(sizeof(int) * listlen);
  int ind = 0;
  for (place = table.begin(); place != table.end(); place++) {
    listcopy[ind].term = place->first;
    listcopy[ind].loc = -1;
    counts[ind] = place->second;
    ind++;
  }
  delete[](list);
  list = listcopy;
}

