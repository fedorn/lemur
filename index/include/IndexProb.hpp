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
#include <cstdio>
#include <cstdlib>
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

