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

#ifndef _MEMLISTH_
#define _MEMLISTH_

class MemList {
public:
  MemList(int incr, int align=8);
  void Reset();
  int NumberOfSegments() const { return numSegments; }
  int Used() const { return (((int)curp) - ((int)p)) + Total(); }
  int Total() const
    {
      int total = ((int)endp) - ((int)p);
      for (MemList *curml = next; curml; curml = curml->next)
	total += ((int)curml->endp) - ((int)curml->p);
      return total;
    }
  ~MemList();
  void *GetMem(int size);

private:
  int numSegments;
  int increment;
  int alignment;
  char *p;
  char *curp;
  char *endp;
  MemList *next;
};

#endif

