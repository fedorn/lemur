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
  if (counts != NULL) {
    free(counts);
    delete[](listcounted);
  }
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

  if (counts) {
    info.freq = counts[index];
    info.loclist = &(listcounted[index].loc);    
    info.tid = listcounted[index].term;
    info.loc = listcounted[index].loc[0];
   
  } else {
    info.freq = 1;
    info.loclist = new vector<int>;
    info.loclist->push_back(list[index].loc);
    info.tid = list[index].term;
    info.loc = list[index].loc;
  }

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

  //already been counted then we don't want to count again.
  if (counts)
    return;

  map<int, LLTerm> table;
  map<int, LLTerm>::iterator place;
  for (int i=0;i<listlen;i++) {
    place = table.find(list[i].term);
    if (place != table.end()) {
      (place->second).term++; // term is keeping a count of the terms
      (place->second).loc.push_back(list[i].loc);
    } else {
      //store to table
      LLTerm* lt = new LLTerm;
      lt->term = 1;
      lt->loc.push_back(list[i].loc);
      table[list[i].term]=*lt;
    }
  }

  listlen = table.size();
  listcounted = new LLTerm[listlen];
  counts = (int*) malloc(sizeof(int) * listlen);
  int ind = 0;
  for (place = table.begin(); place != table.end(); place++) {
    listcounted[ind].term = place->first;
    listcounted[ind].loc = (place->second).loc;
    counts[ind] = (place->second).term;
    ind++;
  }
}

