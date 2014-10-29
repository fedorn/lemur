/*==========================================================================
 * Copyright (c) 2007 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 *
 * 26 Sep 2007 - mjh - initial file start
 */


#ifndef _FIELDINFOLIST_HPP
#define _FIELDINFOLIST_HPP

#include "IndexTypes.hpp"
#include "lemur-platform.h"
#include "Exception.hpp"

namespace lemur
{
  namespace api 
  {
    
    //!  Abstract Representation of Field Information Entry

    /*!

    This is an abstract class that represents the information associated
    with each field entry in a document. The basic information
    includes the begin and end positions of the field, the field ID (field type),
    the field ordinal and the parent ordinal.

    */

    class FieldInfo {
    public:
      int begin;
      int end;
      int id;
      int ordinal;
      int parentOrdinal;
      INT64 number;

      inline FieldInfo() {}

      inline FieldInfo(int _begin, int _end, int _id, INT64 _number=0, int _ordinal=0, int _parentOrdinal=0) :
      begin(_begin), end(_end), id(_id),
      ordinal(_ordinal), parentOrdinal(_parentOrdinal), number(_number) {  }

      inline FieldInfo(const FieldInfo &c) : 
      begin(c.begin), end(c.end), id(c.id),
      ordinal(c.ordinal), parentOrdinal(c.parentOrdinal), number(c.number) {  }


    };


    //!  Abstract Interface of Field Information List

    /*!

    This is an abstract class that represents the FieldInfo entries for a document in an index

    */

    class FieldInfoList {
    public:
      virtual ~FieldInfoList() {}

    protected:
      // Helper functions for iterator, subclasses should override
      /// Create new element of this list type for the iterator
      virtual FieldInfo* newElement() const { return new FieldInfo(); }
      /// Set element from position, returns pointer to the element
      virtual FieldInfo* getElement(FieldInfo* elem, POS_T position) const =0;
      /// Copy values from one Element to another
      /// Subclasses must cast from FieldInfo if not using FieldInfo elements
      virtual void assignElement(FieldInfo* to, FieldInfo* from) const { *to = *from; }
      /// Position at beginning of list
      virtual POS_T beginPosition() const =0;
      /// Position at end of list
      virtual POS_T endPosition() const =0;
      /// Advance to next position
      virtual POS_T nextPosition(POS_T position) const =0;

    public:
      // Single, internal iteration
      /// Prepare iteration
      virtual void startIteration()=0;

      /// Has more entries
      virtual bool hasMore()const=0;

      /// Get a pointer to the next entry (pointer to a local static memory), so do not delete it
      virtual FieldInfo *nextEntry()const=0;

      /// returns the number of FieldInfo elements in the list
      virtual int size()=0;

      /// indexed access to the list
      virtual FieldInfo* operator[] (int index)=0;

      // C++ style forward input (readonly) iterator
      /// Iterator class, used for all subclass lists
      class iterator : std::iterator<std::input_iterator_tag, FieldInfo> {
      public:
        iterator() : list(NULL), position(0), current(NULL) {}
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
        iterator(const FieldInfoList* til, POS_T pos) : list(til), position(pos) {
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

        FieldInfo& operator*() { return *current; }
        FieldInfo* operator->() { return current; }
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
        const FieldInfoList* list;  // list associated with this iterator
        POS_T position;     // current position in list
        FieldInfo* current;   // current element of list
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
      mutable FieldInfoList::iterator itbegin;  // iterator at head of list
      mutable FieldInfoList::iterator itend;    // iterator at end of list
      friend class iterator;
    };
  }
}


#endif
