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

#ifdef __MK__
#pragma mklibrary(Class)
#endif

#pragma implementation

#include <MemList.hpp>
#include <iostream.h>

#ifndef NULL
#define NULL (void *)0
#endif

MemList::MemList(int incr, int align)
{
  numSegments = 1;
  increment = incr;
  alignment = align;
  p = new char[increment];
  curp = p;
  endp = p + increment;
  next = (MemList *)NULL;
#ifdef DEBUG
  cerr << "Allocating memlist: " << (unsigned int)p << endl;
#endif
}

MemList::~MemList()
{
  if (p)
    {
#ifdef DEBUG
      cerr << "Deleting memlist: " << (unsigned int)p << endl;
#endif
      delete [] p;
    }
  if (next)
    delete next;
}

void MemList::Reset()
{
  if (next)
    {
      delete next;
      next = NULL;
    }
  curp = p;
}

void *MemList::GetMem(int size)
{
  int mod = size % alignment;
  if (mod) size += alignment - mod;
  if (curp + size > endp)
    {
      if (size > increment) increment = size;
      MemList *ml = new MemList(increment, alignment);
      // swap p and endp between this and ml
      char *tmp;
      tmp = p; p = ml->p; ml->p = tmp;
      tmp = endp; endp = ml->endp; ml->endp = tmp;
      curp = p;
      ml->next = next;
      next = ml;
      ml->numSegments = numSegments;
      numSegments++;
    }
  void *result = curp;
  curp += size;
  return result;
}
