/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


#ifndef _TERMINFOLIST_HPP
#define _TERMINFOLIST_HPP

#include "IndexTypes.hpp"
#include "Exception.hpp"

//!  Abstract Representation of Term Information Entry

/*!

  This is an abstract class that represents the information associated
with each  term entry in the doc to term index. The basic information
includes  the term ID and the frequency of the term.

*/

class TermInfo {
public:
  TermInfo() {}
  TermInfo( TERMID_T termID, COUNT_T termCount) :
    tid(termID), tcount(termCount) {}
  virtual ~TermInfo() {}

  /// Term id
  virtual TERMID_T termID() const {return tid;}

  /// Set term id
  virtual void termID(TERMID_T id) {tid = id;}

  /// Term count in the doc
  virtual COUNT_T count() const {return tcount;}

  /// Set term count
  virtual void count(COUNT_T c) {tcount = c;}

  // Return list of positions this term occurs in this document
  // (can be a list of 1 item)
  // Default implementation to return NULL if no position information available for this TermInfo
  // List of positions is better used for bag of words support
  virtual const LOC_T* positions() const{ return NULL; }

  // Return position this term occurs in this document
  // Better for sequence of words support
  // When list of positions can be obtained, this returns the first item in the list
  // Default implementation to return -1 if no position information available for this TermInfo
  virtual LOC_T position() const { return -1; }

  virtual void position(LOC_T pos) {}

protected:
  TERMID_T tid;
  COUNT_T tcount;
};


//!  Abstract Interface of Term Information List

/*!

  This is an abstract class that represents the term entries in doc index,
  i.e., the ID and frequency of all terms in a document.

*/

class TermInfoList {
public:
  virtual ~TermInfoList() {}

protected:
  // Helper functions for iterator, subclasses should override
  /// Create new element of this list type for the iterator
  virtual TermInfo* newElement() const { return new TermInfo(); }
  /// Set element from position, returns pointer to the element
  virtual TermInfo* getElement(TermInfo* elem, POS_T position) const =0;
  /// Copy values from one Element to another
  /// Subclasses must cast from TermInfo if not using TermInfo elements
  virtual void assignElement(TermInfo* to, TermInfo* from) const { *to = *from; }
  /// Position at beginning of list
  virtual POS_T beginPosition() const =0;
  /// Position at end of list
  virtual POS_T endPosition() const =0;
  /// Advance to next position
  virtual POS_T nextPosition(POS_T position) const =0;

public:
  // Single, internal iteration
  /// Prepare iteration
  virtual void startIteration()const=0;
  /// Has more entries
  virtual bool hasMore()const=0;
  /// Get a pointer to the next entry (pointer to a local static memory), so do not delete it
  virtual TermInfo *nextEntry()const=0;

  // C++ style forward input (readonly) iterator
  /// Iterator class, used for all subclass lists
  class iterator : std::iterator<std::input_iterator_tag, TermInfo> {
  public:
    iterator() : list(NULL), position(NULL), current(NULL) {}
    iterator(const iterator& other) {
      list = other.list;
      position = other.position;
      if ((list) && (other.current) ) {
	current = list->newElement();
	list->assignElement(current, other.current);  // list knows element class
      } else {
	current = NULL;
      }
    }
    iterator(const TermInfoList* til, POS_T pos) : list(til), position(pos) {
      if (list) {
	if (position != list->endPosition()) {
	  current = list->newElement();   // get new element
	  current = list->getElement(current, position);
	} else {
	  current = NULL;
	}
      }
    }

    ~iterator() {
        delete(current);
    }

    TermInfo& operator*() { return *current; }
    TermInfo* operator->() { return current; }
    iterator& operator++() {
      position = list->nextPosition(position);
      if (position != list->endPosition())
        current = list->getElement(current, position);
      return *this;
    }
    // identical to prefix version
    iterator& operator++(int) {
      return operator++();
    }
    bool operator==(const iterator& other) const {
      return (list == other.list) && (position == other.position);
    }
    bool operator!=(const iterator& other) const {
      return (list != other.list) || (position != other.position);
    }
    iterator& operator=(const iterator& other) {
      list = other.list;
      position = other.position;
      if ((list) && (other.current)) {
	if (!current)
	  current = list->newElement();
	list->assignElement(current, other.current);  // list knows element class
      } else {
	delete(current);
	current=NULL;
      }
      return *this;
    }
    /// seek to a particular place in the list
    /// use with caution. make sure you know what POS_T should be
    void seek(POS_T pos) {
      position = pos;
      if (position != list->endPosition()) {
	if (!current)
	  current = list->newElement();
        current = list->getElement(current, position);
      } else {
	delete(current);
	current = NULL;
      }
    }

  protected:
    const TermInfoList* list;  // list associated with this iterator
    POS_T position;     // current position in list
    TermInfo* current;   // current element of list
  }; // end of nested iterator declaration
 
  iterator& begin() const { 
    iterator it(this, beginPosition());
    itbegin = it;
    return itbegin;
  }
  iterator& end() const { 
    iterator it(this, endPosition());
    itend = it;
    return itend;
  }

protected:
  mutable TermInfoList::iterator itbegin;  // iterator at head of list
  mutable TermInfoList::iterator itend;    // iterator at end of list
  friend class iterator;
};


#endif
