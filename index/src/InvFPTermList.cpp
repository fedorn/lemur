/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/
/*
 * dmf 10/18/2002 -- Add binReadC, binWriteC, deletaEncode, deltaDecode
 * to enable compression of termInfoLists. Added constructor taking a
 * vector of LocatedTerms for list construction.
 */

#include "InvFPTermList.hpp"
#include "File.hpp"

InvFPTermList::InvFPTermList() {
  length = 0;
  index = 0;
  listlen = 0;
  uid = -1;
  list = NULL;
  counts = NULL;
}

InvFPTermList::InvFPTermList(int did, int len, vector<LocatedTerm> &tls) {
  length = len;
  index = 0;
  listlen = tls.size();
  counts = NULL;
  uid = did;
  list = new LocatedTerm[listlen];
  for (int i = 0; i < listlen; i++)
    list[i] = tls[i];
}

InvFPTermList::~InvFPTermList() {
  if (list != NULL)
    delete[](list);
  if (counts != NULL) {
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
  if (counts) {
    entry.freq = counts[index];
    entry.loclist = &(listcounted[index].loc);    
    entry.tid = listcounted[index].term;
    entry.loc = listcounted[index].loc[0];
   
  } else {
    entry.freq = 1;
    ///empty it!
    loclist.clear();
    entry.loclist = &loclist;
    entry.loclist->push_back(list[index].loc);
    entry.tid = list[index].term;
    entry.loc = list[index].loc;
  }

  index++;
  return &entry;
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

//
// File-based functions
//

void InvFPTermList::binWriteC( File& of ) {
  of.write((const char*) &uid, sizeof(DOCID_T));
  of.write((const char*) &length, sizeof(int));
  //  if (length == 0) {
  if (listlen == 0) {
    int zero = 0;
    of.write((const char*) &zero, sizeof(int));
    return;
  }
  
  deltaEncode();
  // compress it
  // it's ok to make comp the same size.  the compressed will be smaller
  unsigned char* comp = new unsigned char[listlen * sizeof(LocatedTerm)];
  int compbyte = RVLCompress::compress_ints((int *)list, comp, 
					    listlen * 2);
  of.write((const char*) &compbyte, sizeof(int));
  // write out the compressed bits
  of.write((const char*) comp, compbyte);
  delete[](comp);
}

bool InvFPTermList::binReadC( File& infile ){
  int size = 0;
  
  infile.read((char*) &uid, sizeof(DOCID_T));
  infile.read((char*) &length, sizeof(int));
  infile.read((char*) &size, sizeof(int));

  if (size == 0) {
    list = new LocatedTerm[0];
    listlen = 0;
    return true;
  }
  
  unsigned char *buffer = new unsigned char[size];
  infile.read((char *)buffer, size);

  if (!(list == NULL))
    delete[](list);

  list = new LocatedTerm[size * 4];
  // decompress it
  listlen = RVLCompress::decompress_ints(buffer, (int *)list, size)/2;
  deltaDecode();
  delete[](buffer);
  return true;
}

//
// Stream-based functions
// 

void InvFPTermList::binWriteC(ofstream& of) {
  of.write((const char*) &uid, sizeof(DOCID_T));
  of.write((const char*) &length, sizeof(int));
  //  if (length == 0) {
  if (listlen == 0) {
    int zero = 0;
    of.write((const char*) &zero, sizeof(int));
    return;
  }
  
  deltaEncode();
  // compress it
  // it's ok to make comp the same size.  the compressed will be smaller
  unsigned char* comp = new unsigned char[listlen * sizeof(LocatedTerm)];
  int compbyte = RVLCompress::compress_ints((int *)list, comp, 
					    listlen * 2);
  of.write((const char*) &compbyte, sizeof(int));
  // write out the compressed bits
  of.write((const char*) comp, compbyte);
  delete[](comp);
}

bool InvFPTermList::binReadC(ifstream& infile){
  int size;
  
  if (infile.eof())
    return false;

  infile.read((char*) &uid, sizeof(DOCID_T));
  if (!(infile.gcount() == sizeof(DOCID_T)))
    return false;

  infile.read((char*) &length, sizeof(int));
  if (!(infile.gcount() == sizeof(int)))
    return false;

  infile.read((char*) &size, sizeof(int));
  if (!(infile.gcount() == sizeof(int)))
    return false;
  if (size == 0) {
    list = new LocatedTerm[0];
    listlen = 0;
    return true;
  }
  
  unsigned char *buffer = new unsigned char[size];
  infile.read((char *)buffer, size);
  if (!infile.gcount() == size) {
    return false;
  }

  if (!(list == NULL))
    delete[](list);

  list = new LocatedTerm[size * 4];
  // decompress it
  listlen = RVLCompress::decompress_ints(buffer, (int *)list, size)/2;
  deltaDecode();
  delete[](buffer);
  return true;
}


void InvFPTermList::deltaEncode() {
  // we will encode in place
  // go backwards starting at the last docid
  // we're counting on two always being bigger than one
  int* two = (int *)(list + listlen - 1);
  int* one = two-2;

  while (two != (int *)list) {
    *two = *two-*one;
    two = one;
    one -= 2;
  }
}

void InvFPTermList::deltaDecode() {
  // we will decode in place
  // start at the begining
  int* one = (int *)list;
  int* two = one+2;
  
  while (one != (int *)(list + listlen - 1)) {
    *two = *two + *one;
    one = two;
    two += 2;
  }
}

void InvFPTermList::countTerms(){
  //already been counted then we don't want to count again.
  if (counts || listlen == 0) // pathological case of 0 length list.
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
