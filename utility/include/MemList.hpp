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

