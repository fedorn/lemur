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


#ifndef _FLL_H
#define _FLL_H

#include <cstdlib>
#include <cassert>
#include <MemList.hpp>

template <class Type> class FastList;

template <class Type>
class FLL {
public:
friend class FastList<Type>;

  FLL() : data(NULL), next(NULL) {} // default constructor
  FLL(Type &d) : data(&d), next(NULL) {}
  FLL(Type &d, FLL<Type> *pNext) : data(&d), next(pNext) {}
  ~FLL() {}			// destructor does nothing, since using MemList

  FLL<Type> *Next() const { return next; }
  void Replace(Type &newData) { data = &newData; }
  Type &Data() const { return *data; }

private:
  Type *data;			// pointer to data at this node
  FLL<Type> *next;		// pointer to next node in list

  // Disallow these default operations
  FLL(const FLL &) { assert(0); }
  FLL &operator=(const FLL &) { assert(0); return *this; }
  int operator==(const FLL &x) const { return this == &x; }

public:
  void *operator new(size_t size) { return pMemList->GetMem(size); }
  void operator delete(void *) { }
  static void UseMemory(MemList &memList) { pMemList = &memList; }

private:
  static MemList *pMemList;
};

#endif
