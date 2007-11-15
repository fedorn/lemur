#include "IndriFieldInfoList.hpp"

using namespace lemur::api;

/** C'tor / D'tor methods **/

/// C'tor - initializes a blank list
IndriFieldInfoList::IndriFieldInfoList() {
}

/// C'tor - initializes a list with field extents
IndriFieldInfoList::IndriFieldInfoList(const indri::utility::greedy_vector< indri::index::FieldExtent > &fields) {
}

IndriFieldInfoList::~IndriFieldInfoList() { 
}

/** protected methods **/

/// Set element from position, returns pointer to the element
FieldInfo* IndriFieldInfoList::getElement(FieldInfo* elem, POS_T position) const {
  if (position < 0 || position >= _list.size()) { 
    return NULL;
  }

  FieldInfo thisItem=_list[position];
  elem->begin=thisItem.begin;
  elem->end=thisItem.end;
  elem->id=thisItem.id;
  elem->number=thisItem.number;
  elem->ordinal=thisItem.ordinal;
  elem->parentOrdinal=thisItem.parentOrdinal;

  return elem;
}

/// Position at beginning of list
POS_T IndriFieldInfoList::beginPosition() const {
  return 0;
}

/// Position at end of list
POS_T IndriFieldInfoList::endPosition() const {
  return (_list.size()-1);
}

/// Advance to next position
POS_T IndriFieldInfoList::nextPosition(POS_T position) const {
  return (position+1);
}

/** public methods **/

/// appends a new field entry at the end of the list
void IndriFieldInfoList::add(indri::index::FieldExtent newExtent) {
  FieldInfo newItem;
  newItem.begin=newExtent.begin;
  newItem.end=newExtent.end;
  newItem.id=newExtent.id;
  newItem.number=newExtent.number;
  newItem.ordinal=newExtent.ordinal;
  newItem.parentOrdinal=newExtent.parentOrdinal;
  _list.push_back(newItem);
}

// Single, internal iteration
/// Prepare iteration
void IndriFieldInfoList::startIteration() {
  _position=0;
}

/// Has more entries
bool IndriFieldInfoList::hasMore() const {
  return (_position < _list.size());
}

/// Get a pointer to the next entry (pointer to a local static memory), so do not delete it
lemur::api::FieldInfo *IndriFieldInfoList::nextEntry() const {
  if (_position < _list.size()) {
    getElement( &const_cast<IndriFieldInfoList*>(this)->_currentInfo, lemur::api::POS_T(_position) );
    const_cast<IndriFieldInfoList*>(this)->_position++;
    return &const_cast<IndriFieldInfoList*>(this)->_currentInfo;
  } 
  return NULL;
}

/// returns the number of FieldInfo elements in the list
int IndriFieldInfoList::size() {
  return _list.size();
}

/// indexed access to the list
lemur::api::FieldInfo* IndriFieldInfoList::operator[] (int index) {
  if (index < 0 || index >= _list.size()) { 
    _position=_list.size();
    return NULL;
  }
  _position=index;
  return this->nextEntry();
}
