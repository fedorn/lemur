/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#ifndef _DOCPOSITIONINFOLIST_HPP
#define _DOCPOSITIONINFOLIST_HPP

#include "DocInfoList.hpp"
class DocPositionInfoList : public DocInfoList {
public:
  /// fetch the next entry with docID >= <documentID>
  virtual DocInfo* nextEntry( DOCID_T documentID ) = 0;
  // why is this required?
  virtual DocInfo* nextEntry() const = 0;
  virtual DocInfo* currentEntry() = 0;
protected:
  // Helper functions for iterator, subclasses should override
  /// Create new element of this list type for the iterator
  virtual DocInfo* newElement() const = 0;
  /// copy values from one Element (of type DocInfo) to another
  virtual void assignElement(DocInfo* to, DocInfo* from) const = 0;
  /// set element from position, returns pointer to the element
  virtual DocInfo* getElement(DocInfo* elem, POS_T position) const = 0;
  /// Position at beginning of list
  virtual POS_T beginPosition() const = 0;
  /// Position at end of list
  virtual POS_T endPosition() const = 0;
  /// Advance to next position
  virtual POS_T nextPosition(POS_T position) const = 0;
};
#endif

