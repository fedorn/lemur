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


#ifndef _FASTLISTH_
#define _FASTLISTH_

#include <cstdlib>
#include <cassert>
#include <Null.hpp>
#include <MemList.hpp>
#include <FLL.hpp>

template <class Type>
class FastList {
public:
  FastList() : head(NULL), tail(NULL) {} // default constructor
  ~FastList() {}			 // destructor does nothing
  void Clear() { head = tail = NULL; }	 // clear list without deleting

  int operator!() const { return head == NULL; }

  FastList<Type> &AddToHead(Type &d)
  {
    if (!head) { head = tail = new FLL<Type>(d); }
    else
      { FLL<Type> *tmp = new FLL<Type>(d); tmp->next = head; head = tmp; }
    return *this;
  }

  FastList<Type> &AddToTail(Type &d)
  {
    if (!tail) {
      tail = head = new FLL<Type>(d);
    }
    else {
      tail->next = new FLL<Type>(d); tail = tail->next; 
    }
    return *this;
  }

  Type &PopHead()
  {
    if (head == NULL) return null(Type);
    Type &d = head->Data();
    head = head->next;
    if (head == NULL)
      tail = NULL;
    return d;
  }

  int ComputeLength() const
  { int l=0; for (FLL<Type> *ll=head; ll; ll=ll->Next()) l++; return l; }

  // Accessor Functions
  FLL<Type> *HeadLL() const { return head; }

private:
  FLL<Type> *head;
  FLL<Type> *tail;

public:   // Globals
  // Memory management
  void *operator new(size_t size) { return pMemList->GetMem(size); }
  void operator delete(void *) { }
  static void UseMemory(MemList &memList)
  {
    FLL<Type>::UseMemory(memList);
    pMemList = &memList;
  }
private:
  static MemList *pMemList;
};

#define FMAPCAR(list, ll) for (ll = (list).HeadLL(); ll; ll = ll->Next())

#endif
