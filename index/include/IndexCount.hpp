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


#ifndef _INDEXCOUNTH_
#define _INDEXCOUNTH_

///  class for collecting counts of an index

/*!

 A class for collecting counts of an index.  Used for 
 counting and sorting occurrences of terms and documents.
*/


// J. Lafferty, 10/00
//
//---------------------------------------------------------------


#include <fstream.h>
#include <cstdio>
#include <cstdlib>
#include "MemList.hpp"

class IndexCount
{
public:
  IndexCount() : index(0), count(0) {};
  IndexCount(int id, int ct) : index(id), count(ct) {}
  IndexCount(IndexCount &dc) : index(dc.index), count(dc.count) {}
  IndexCount &operator=(const IndexCount &that)
    { index=that.index; count=that.count; return *this; }
  int operator==(const IndexCount &that) const
    { return ((index==that.index) && (count==that.count)); }
  int operator!=(const IndexCount &that) const
    { return ((index!=that.index) || (count!=that.count)); }
  int operator>(const IndexCount &that) const { return count > that.count; }
  void read(ifstream &ifs) { 
    ifs.read(&index, sizeof(int));
    ifs.read(&count, sizeof(int));
  }
  void write(ofstream &ofs) const { 
    ofs.write(&index, sizeof(int));
    ofs.write(&count, sizeof(int));
  }

public:
  int index;
  int count;

// Sorting routines -- quicksort using random pivoting
public:
  static int rqSort(int n, IndexCount * ic);

private:
  static void insertionsort(IndexCount * a, int n);
  static void rquicksort(IndexCount * a, int n);

public:
  // Memory management
  void *operator new(size_t size) { return pMemList->GetMem(size); }
  void operator delete(void *) { }
  static void UseMemory(MemList &memList);

private:
  static MemList *pMemList;

};

#endif

