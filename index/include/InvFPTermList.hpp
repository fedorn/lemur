 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef _INVFPTERMLIST_HPP
#define _INVFPTERMLIST_HPP

#include <iostream>
#include <fstream>
#include "InvFPTerm.hpp"
#include "InvFPTypes.hpp"
#include "map.hpp"

/*! 

  This is a class that represents the term entries in doc index, to be used with InvFPIndex
  i.e., the ID and frequency of all terms in a document.

*/

class InvFPTermList : public TermInfoList {
public:
  InvFPTermList();
  ~InvFPTermList();

  /// prepare iteration
  void startIteration();

  /// has more entries
  bool hasMore();

  /// Get a pointer to the next entry (pointer to a local static memory)
  TermInfo *nextEntry();

  /// Get the length of this document
  int docLength(){ return length; }

  /// Get the number of terms in this document
  int termCount() { return listlen; }

  /// Get the docid this list is for
  int docID() { return uid; }

  /// Read in a TermInfoList object from a file
  /// Assumes the following format: DOCID DOCLENGTH UNIQUE_TERM_COUNT [TERM LOC]..[]
  bool binRead(ifstream& infile);

private:

  DOCID_T uid; // this doc's id
  int length;  // length of this document (terms + stopwords)
  LocatedTerm* list; // list of terms and locations
  int listlen; // number of items we have in list  (same as number of terms)
  int index;   // index for iterator
};




#endif
