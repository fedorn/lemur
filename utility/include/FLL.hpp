 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _FLL_H
#define _FLL_H

#include <stdlib.h>
#include <assert.h>
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
