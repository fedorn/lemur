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

InvFPTermList::InvFPTermList(DOCID_T did, int len, vector<LocatedTerm> &tls) {
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

void InvFPTermList::startIteration() const{
  index = 0;
}

bool InvFPTermList::hasMore() const{
  return index != listlen;
}

TermInfo* InvFPTermList::nextEntry() const{
  if (counts) {
    entry.count(counts[index]);
    entry.positions(&(listcounted[index].loc));
    entry.termID(listcounted[index].term);
    entry.position(listcounted[index].loc[0]);

  } else {
    entry.count(1);
    ///empty it!
    loclist.clear();
    loclist.push_back(list[index].loc);
    entry.positions(&loclist);
    entry.termID(list[index].term);
    entry.position(list[index].loc);
  }

  index++;
  return &entry;
}

/// set element from position, returns pointer to the element
TermInfo* InvFPTermList::getElement(TermInfo* elem, POS_T position) const {
  InvFPTerm* e = dynamic_cast<InvFPTerm*>(elem);
  if (!e)
    LEMUR_THROW(LEMUR_RUNTIME_ERROR,"expected InvFPTerm");
  
  int index = (int) position;

  if (counts) {
    e->count(counts[index]);
    e->positions(&(listcounted[index].loc));
    e->termID(listcounted[index].term);
    e->position(listcounted[index].loc[0]);

  } else {
    e->count(1);
    ///empty it!
    loclist.clear();
    loclist.push_back(list[index].loc);
    e->positions(&loclist);
    e->termID(list[index].term);
    e->position(list[index].loc);
  }
  return e;
}
/// advance position
POS_T InvFPTermList::nextPosition(POS_T position) const {
  return (POS_T) (((int) position) + 1);
}

bool InvFPTermList::binRead(ifstream& infile){
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
  of.write((const char*) &length, sizeof(COUNT_T));
  //  if (length == 0) {
  if (listlen == 0) {
    COUNT_T zero = 0;
    of.write((const char*) &zero, sizeof(COUNT_T));
    return;
  }
  
  deltaEncode();
  // compress it
  // it's ok to make comp the same size.  the compressed will be smaller
  unsigned char* comp = new unsigned char[listlen * sizeof(LocatedTerm)];
  COUNT_T compbyte = RVLCompress::compress_ints((int *)list, comp, 
					    listlen * 2);
  of.write((const char*) &compbyte, sizeof(COUNT_T));
  // write out the compressed bits
  of.write((const char*) comp, compbyte);
  delete[](comp);
}

bool InvFPTermList::binReadC( File& infile ){
  COUNT_T size = 0;
  
  infile.read((char*) &uid, sizeof(DOCID_T));
  infile.read((char*) &length, sizeof(COUNT_T));
  infile.read((char*) &size, sizeof(COUNT_T));

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

void InvFPTermList::binWriteC(ofstream& of){
  of.write((const char*) &uid, sizeof(DOCID_T));
  of.write((const char*) &length, sizeof(COUNT_T));
  //  if (length == 0) {
  if (listlen == 0) {
    COUNT_T zero = 0;
    of.write((const char*) &zero, sizeof(COUNT_T));
    return;
  }
  
  deltaEncode();
  // compress it
  // it's ok to make comp the same size.  the compressed will be smaller
  unsigned char* comp = new unsigned char[listlen * sizeof(LocatedTerm)];
  COUNT_T compbyte = RVLCompress::compress_ints((int *)list, comp, 
					    listlen * 2);
  of.write((const char*) &compbyte, sizeof(COUNT_T));
  // write out the compressed bits
  of.write((const char*) comp, compbyte);
  delete[](comp);
}

bool InvFPTermList::binReadC(ifstream& infile){
  COUNT_T size;
  
  if (infile.eof())
    return false;

  infile.read((char*) &uid, sizeof(DOCID_T));
  if (!(infile.gcount() == sizeof(DOCID_T)))
    return false;

  infile.read((char*) &length, sizeof(COUNT_T));
  if (!(infile.gcount() == sizeof(COUNT_T)))
    return false;

  infile.read((char*) &size, sizeof(COUNT_T));
  if (!(infile.gcount() == sizeof(COUNT_T)))
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
  LOC_T* two = (LOC_T *)(list + listlen - 1);
  LOC_T* one = two-2;

  while (two != (LOC_T *)list) {
    *two = *two-*one;
    two = one;
    one -= 2;
  }
}

void InvFPTermList::deltaDecode() {
  // we will decode in place
  // start at the begining
  LOC_T* one = (LOC_T *)list;
  LOC_T* two = one+2;
  
  while (one != (LOC_T*)(list + listlen - 1)) {
    *two = *two + *one;
    one = two;
    two += 2;
  }
}

void InvFPTermList::countTerms(){
  //already been counted then we don't want to count again.
  if (counts || listlen == 0) // pathological case of 0 length list.
    return;
  map<TERMID_T, COUNT_T> table;
  map<TERMID_T, COUNT_T>::iterator place;
  int idx = 0;
  listcounted = new LLTerm[listlen];
  //  counts = (int*) malloc(sizeof(int) * listlen);
  counts = new COUNT_T[listlen];
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
