#ifndef _INVFPINDEXREADER_HPP
#define _INVFPINDEXREADER_HPP

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "InvFPTypes.hpp"
#include "InvFPDocList.hpp"

class InvFPIndexReader{
public:
	InvFPIndexReader();
	InvFPIndexReader(string fname);
  InvFPIndexReader(char* fname);
	~InvFPIndexReader();

	void setFStream(FILE* fstream);
	void setFStream(string fname);

  /// end of index
  bool EOIndex();
  /// get the doclist for this term
  /// caution, this list will be destroyed when the reader object is destroyed
  /// if you need to do keep it around, make a copy
	InvFPDocList* readNextList();
  InvFPDocList* currentList();
private:
	FILE* reader;
  InvFPDocList* current; // the list we've most recently read in
};

#endif