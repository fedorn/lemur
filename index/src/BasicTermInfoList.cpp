#include "BasicTermInfoList.hpp"

BasicTermInfoList::BasicTermInfoList(int *tmpWordArray, int size):
  tmpwarr(tmpWordArray), sz(size), it(0)
{
}

/// This function is implemented inline, because it gets called frequently
TermInfo * BasicTermInfoList::nextEntry() const{
  // assert (hasMore());
  static TermInfo info;
  info.termID(tmpwarr[it]);
  info.count(tmpwarr[sz+it]);
  it++;
  return (&info);
}

/// set element from position, returns pointer to the element
TermInfo* BasicTermInfoList::getElement(TermInfo* elem, POS_T position) const {
  int i = (int) position;
  elem->termID(tmpwarr[i]);
  elem->count(tmpwarr[sz+i]);
  return elem;
}
/// advance position
POS_T BasicTermInfoList::nextPosition(POS_T position) const {
  return (POS_T) (((int) position) + 1);
}




