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


#ifndef _INVFPINDEXMERGE_HPP
#define _INVFPINDEXMERGE_HPP

#include "vector.hpp"
#include "map.hpp"
#include "InvFPDocList.hpp"
#include "InvFPTypes.hpp"

#define READBUFSIZE 2000000

struct IndexReader {
  InvFPDocList* list;
  ifstream* reader;
};

// this class could actually be static
class InvFPIndexMerge {
public:
  /// constructor.  
  /// vector : a list of files to merge
  /// char* : name prefix of created files
  /// long  : the maximum size the index can be
  InvFPIndexMerge(char* buffer, long size, long maxfilesize=2100000000);
  InvFPIndexMerge(long buffersize=64000000, long maxfilesize=2100000000);
  ~InvFPIndexMerge();

  /// output of this merge operation and the lookup table for the merged index
  /// returns the number of index files this merge created
  int merge(vector<char*>* tf, char* prefix);

  void setMaxFileSize(long size);
  char* setBuffer(char* buffer, long size);

  /// recursive hierarchical merge
  /// calls mergeFiles() when intermediate files are necessary
  /// base case if finalMerge()
  int hierMerge(vector<char*>* files, int level);

  /// merge these files and put the results into the intmed list
  int mergeFiles(vector<char*>* files, vector<char*>* intmed, int level);

  /// do the final merge and write the lookup table
  int finalMerge(vector<char*>* files);

private:
  /// write file ids for indexes created
  void writeInvFIDs();
  /// figure out which readers point to the lowest termids
  void least(vector<IndexReader*>* r, vector<int>* ret);

  char* name;
  vector<char*> invfiles; // list of files that we've written to
  long maxfile; // maximum file size for each index
  long bufsize;
  char* readbuffer;
};

#endif
