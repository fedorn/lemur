#ifndef _INVFPINDEXMERGE_HPP
#define _INVFPINDEXMERGE_HPP

#include "vector.hpp"
#include "map.hpp"
#include "InvFPIndexReader.hpp"

// this class could actually be static
class InvFPIndexMerge {
public:
  InvFPIndexMerge(vector<char*>* tf);
  ~InvFPIndexMerge();

  /// name is the prefix for the 2 files 
  /// output of this merge operation and the lookup table for the merged index
  void merge(char* name);

private:
  void least(vector<int>* ret);

  vector<InvFPIndexReader*> readers;  // a list of indexreaders for each file that is being merged
};

#endif