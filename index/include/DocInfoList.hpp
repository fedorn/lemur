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


#ifndef _DOCINFOLIST_HPP
#define _DOCINFOLIST_HPP

#include "IndexTypes.hpp"
#include <iterator>


//!  Representation of Doc Information Entry

/*!

This is a class that represents the information associated
with each doc entry in the term to doc index. The basic information
includes the doc ID and the frequency of a term in the document.

*/

class DocInfo {
public:
  DocInfo() {}
  DocInfo(DOCID_T docID, COUNT_T termCount) :
    did(docID), tcount(termCount) {}

  virtual ~DocInfo() {}

  /// Doc id
  virtual DOCID_T docID() const {return did;}

  /// Set doc id
  virtual void docID(DOCID_T id) {did = id;}

  /// Term count in the doc
  virtual COUNT_T termCount() const {return tcount;}

  /// Set term count
  virtual void termCount(COUNT_T c) {tcount = c;}

  /// Return list of positions this term occurs in this document
  /// Default implementation to return NULL if no position information available for this DocInfo
  virtual const LOC_T* positions() const { return NULL; }

  /// Set the list of positions
  /// Default implementation does nothing
  virtual void positions(const LOC_T* pos) {}
  // Maybe throw an exception here since this shouldn't ever be called

protected:
  DOCID_T did;    // document id
  COUNT_T tcount; // term count
};



//!  Abstract Interface of Doc Information List

/*!

  This is an abstract class that represents the doc entries in term index,
  i.e., the doc ID and the frequency of a term in the document.

*/

class DocInfoList {
public:
  DocInfoList() {}
  virtual ~DocInfoList() {}

protected:
  // Helper functions for iterator, subclasses should override
  /// Create new element of this list type for the iterator
  virtual DocInfo* newElement() const { return new DocInfo(); }
  /// Set element from position, returns pointer to the element
  virtual DocInfo* getElement(DocInfo* elem, POS_T position) const =0;
  /// Copy values from one Element to another
  /// Subclasses must cast from DocInfo if not using DocInfo elements
  virtual void assignElement(DocInfo* to, DocInfo* from) const { *to = *from; }
  /// Position at beginning of list
  virtual POS_T beginPosition() const =0;
  /// Position at end of list
  virtual POS_T endPosition() const =0;
  /// Advance to next position
  virtual POS_T nextPosition(POS_T position) const =0;

public:
  // Single, internal iteration
  /// prepare iteration
  virtual void startIteration() const=0;
  /// test if there's any entry
  virtual bool hasMore() const=0;
  /// fetch the next entry, return a pointer to a local static memory, so do not delete it
  virtual DocInfo *nextEntry() const=0;

  // C++ style forward input (readonly) iterator
  /// Iterator class, used for all subclass lists
  class iterator : std::iterator<std::input_iterator_tag, DocInfo> {
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
    iterator(const DocInfoList* dil, POS_T pos) : list(dil), position(pos) {
      if (list) {
	if (position != list->endPosition()) {
	  current = list->newElement();   // get new element
	  current = list->getElement(current, position);
	} else {
	  current=NULL;
	}
      }
    }


    ~iterator() {
        delete(current);
    }

    DocInfo& operator*() { return *current; }
    DocInfo* operator->() { return current; }
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
	current = NULL;
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
    const DocInfoList* list;  // list associated with this iterator
    POS_T position;     // current position in list
    DocInfo* current;   // current element of list
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
  mutable DocInfoList::iterator itbegin;  // iterator at head of list
  mutable DocInfoList::iterator itend;    // iterator at end of list
  friend class iterator;
};


#endif
