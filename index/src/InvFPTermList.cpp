/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
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
    //    free(counts);
    delete[](counts);
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
  static vector<int> loclist;
  
  if (counts) {
    info.freq = counts[index];
    info.loclist = &(listcounted[index].loc);    
    info.tid = listcounted[index].term;
    info.loc = listcounted[index].loc[0];
   
  } else {
    info.freq = 1;
    //    info.loclist = new vector<int>;
    ///empty it!
    loclist.clear();
    info.loclist = &loclist;
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
#if 0
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
      LLTerm lt;
      lt.term = 1;
      lt.loc.push_back(list[i].loc);
      table[list[i].term]=lt;
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
#endif

void InvFPTermList::countTerms(){
  //already been counted then we don't want to count again.
  if (counts)
    return;
  map<int, int> table;
  map<int, int>::iterator place;
  int idx = 0;
  listcounted = new LLTerm[listlen];
  //  counts = (int*) malloc(sizeof(int) * listlen);
  counts = new int[listlen];
  // Reduce computations at the cost of wasting the end
  // of the two arrays, listcounted and counts
  int i;
  for (i = 0; i < listlen; i++) {
    place = table.find(list[i].term);
    if (place != table.end()) {
      counts[(place->second)]++;
      listcounted[(place->second)].loc.push_back(list[i].loc);
    } else {
      listcounted[idx].term = list[i].term;
      listcounted[idx].loc.push_back(list[i].loc);
      counts[idx] = 1;
      table[list[i].term]=idx;
      idx++;
    }
  }  
  listlen = idx;
}
