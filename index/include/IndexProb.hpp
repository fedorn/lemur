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

#ifndef _INDEXPROB_
#define _INDEXPROB_


/// A class for collecting probabilities for an index


/*!

 A class for collecting probabilities for an index.  Used for 
 counting and sorting scores of terms and documents.

*/

// J. Lafferty, 10/00
//
//---------------------------------------------------------------


#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include "MemList.hpp"

class IndexProb
{
public:
  IndexProb() : index(0), prob(0) {};
  IndexProb(int id, double ct) : index(id), prob(ct) {}
  IndexProb(IndexProb &dc) : index(dc.index), prob(dc.prob) {}
  IndexProb &operator=(const IndexProb &that)
    { index=that.index; prob=that.prob; return *this; }
  int operator==(const IndexProb &that) const
    { return ((index==that.index) && (prob==that.prob)); }
  int operator!=(const IndexProb &that) const
    { return ((index!=that.index) || (prob!=that.prob)); }
  int operator>(const IndexProb &that) const { return prob > that.prob; }
  void read(ifstream &ifs) { 
    ifs.read(&index, sizeof(int));
    ifs.read(&prob, sizeof(double));
  }
  void write(ofstream &ofs) const { 
    ofs.write(&index, sizeof(int));
    ofs.write(&prob, sizeof(double));
  }

public:
  int index;
  double prob;

// Sorting routines -- quicksort using random pivoting
public:
  static int rqSort(int n, IndexProb * ic);

private:
  static void insertionsort(IndexProb * a, int n);
  static void rquicksort(IndexProb * a, int n);

public:
  // Memory management
  void *operator new(size_t size) { return pMemList->GetMem(size); }
  void operator delete(void *) { }
  static void UseMemory(MemList &memList);

private:
  static MemList *pMemList;

};

#endif

