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


#ifndef _LISTH_
#define _LISTH_

#pragma interface

#include <LL.hpp>
#include <Null.hpp>
#include "common_headers.hpp"
#include <cassert>

#define LIST_NOTFOUND (-1)

template <class Type>
class List {
public:
  // ****************** Constructors ***************************
  // Zero-argument constructor
  List() : head((LL<Type> *) NULL), tail((LL<Type> *) NULL), cur((LL<Type> *) NULL), lengthmem(0) { }

  // Copy constructor
  List(const List<Type> &l) : head((LL<Type> *) NULL), tail((LL<Type> *) NULL), cur((LL<Type> *) NULL) { *this = l; }

  // ****************** Destructor ***************************
  // destructor: delete list elements
  ~List();

  // A method to clear data so it is not deleted when list is deleted
  void ClearData();
  // A method to delete data without deleting list
  void DeleteData();

  // ****************** Overloaded Operators ******************
  // Create a unique copy of the list and it's data
  List<Type> &operator=(const List<Type> &l)
    { CopyList(l); DuplicateData(); return *this; }

  int operator!=(List<Type> &l) const { return (*this == l); }
  int operator==(const List<Type> &l) const;

  // Add to head
  List<Type> &operator+=(Type &d) { return AddToHead(d); }
  List<Type> &operator+=(Type *pD) { return AddToHead(pD); }
  List<Type> &operator<<(Type &d) { return AddToHead(d); }
  List<Type> &operator<<(Type *pD) { return AddToHead(pD); }
  List<Type> &AddToHead(Type &d);
  List<Type> &AddToHead(Type *pD);

  // Add to tail
  List<Type> &AddToTail(Type &d);
  List<Type> &AddToTail(Type *pD);

  // Add relative to Current()
  void AddAfter(const Type &d);
  void AddAfter(Type *d);
  void AddBefore(const Type &d);
  void AddBefore(Type *d);

  // Replace Current
  void Replace(const Type &d);
  void Replace(Type *d);

  // Append l to end of this list and clear l
  List<Type> &operator+=(List<Type> &l);

  // Concatenate lists and return result, ClearDatas arguments
  List<Type> operator+(List<Type> &);

  // Remove Functions
  Type &PopHead();
  Type &PopTail();
  // Remove and delete datum d
  List<Type> &operator-=(Type &d);
  // Remove datum d (but don't delete it)
  List<Type> &operator-=(Type *d) { RemoveDatum(d); return *this; }

  // Accessor functions
  Type &Find(const Type &d) const;
  Type &Find(const Type *d) const { return Find(*d); }

  int operator[](const Type &d) const;
  int operator[](const Type *d) const { return (*this)[d]; }
  Type &operator[](int idx) const
    { return ((idx >= lengthmem) || (idx < 0)) ? null(Type) : *(*head)[idx]; }

  // Functions for Traversing list
  Type &GoTo(const Type &d)
    { return (cur = (*head)[&d]) ? *cur->datamem : null(Type); }
  Type &GoTo(int idx)
    {
      cur = head;
      while ((idx-- > 0) && cur) cur = cur->nextmem;
      return cur ? *cur->datamem : null(Type);
    }
  void GoToHead() { cur = head; }
  void GoToTail() { cur = tail; }
  void GoToNext() { cur = cur->nextmem; }
  void GoToPrev() { cur = cur->prevmem; }
  int EndOfList() { return (!cur); }

  // ************************ Accessor Functions *******************
  Type &Head() const { return *head->datamem; }
  Type &Tail() const { return *tail->datamem; }
  Type &Current() { return *cur->datamem; }
  int Length() const { return lengthmem; }
private:
  // Private method for copying from list
  void CopyList(const List<Type> &l);
  // Private method for replacing data in list with duplicate copy
  void DuplicateData();
  // Private method for removing a datum from the list, returns ref to datum
  Type *RemoveDatum(Type *d);

  // ********************** Member Data ***********************
  LL<Type> *head;
  LL<Type> *tail;
  LL<Type> *cur;
  int lengthmem;  // Was unsigned int; changed to avoid g++ warning -- JDL
};

#endif


