#include "InvFPTermPropList.hpp"

bool InvFPTermPropList::hasMoreTerm() const {
  return (nextTermPos() != -1);
}

TermInfo* InvFPTermPropList::nextTerm()  const{
  // we'll do this only for sequence of terms lists
  if (counts)
    return NULL;

  // advance pointer until we get something with a different position
  while ((index < listlen) && (list[index].loc == entry.position())) {
    index++;
  }

  if (index == listlen)
    return NULL;

  entry.count(1);
  loclist.clear();
  loclist.push_back(list[index].loc);
  entry.positions(&loclist);
  entry.termID(list[index].term);
  entry.position(list[index].loc);
  index++;
  return &entry;
}

void InvFPTermPropList::skipTo(int pos)  const{
  // we'll do this only for sequence of terms lists
  if (counts)
    return;

  // advance pointer until we get to given pos, doesn't do anything if current pos
  // is already past point of given pos
  // goes to next available term past pos if no actually pos is found (stopwords)
  // goes to end if pos is beyond end of list
  while ((index < listlen) && (list[index].loc < pos)) {
    index++;
  }
}

int InvFPTermPropList::nextEntryPos()  const{
  if (counts)
    return -1;
  if (index == listlen)
    return -1;
  return list[index].loc;
}

int InvFPTermPropList::nextTermPos()  const{
  if (counts)
    return -1;

  int tracker = index;
  while ((tracker < listlen) && (list[tracker].loc == entry.position())) {
    tracker++;
  }

  if (tracker == listlen)
    return -1;
  return list[tracker].loc;
}
