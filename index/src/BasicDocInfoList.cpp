#include "BasicDocInfoList.hpp"

BasicDocInfoList::BasicDocInfoList(int *tmpDocArray, int size):
  tmpdarr(tmpDocArray), sz(size), it(0), prevDocID(0) 
{
}

DocInfo * BasicDocInfoList::nextEntry() const{
  // assert (hasMore());
  static DocInfo info;
  info.docID(tmpdarr[it]+prevDocID);
  prevDocID = info.docID();
  info.termCount(tmpdarr[sz+it]);
  it++;
  return (&info);
}

/// set element from position, returns pointer to the element
DocInfo* BasicDocInfoList::getElement(DocInfo* elem, POS_T position) const {
  int i = (int) position;
  elem->termCount(tmpdarr[sz+i]);
  int id = 0;
  for (;i >= 0; --i)
    id+=tmpdarr[i];
  elem->docID(id);
  return elem;
}
/// advance position
POS_T BasicDocInfoList::nextPosition(POS_T position) const {
  return (POS_T) (((int) position) + 1);
}




