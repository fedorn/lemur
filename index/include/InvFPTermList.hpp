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

/*
  10/18/2002 -- dmf Add binReadC, binWriteC, deltaDecode, and deltEncode
  for compression of TermInfoLists.
 */

#ifndef _INVFPTERMLIST_HPP
#define _INVFPTERMLIST_HPP

#include "common_headers.hpp"
#include "InvFPTerm.hpp"
#include "InvFPTypes.hpp"
#include "RVLCompress.hpp"

/*! 

  This is a class that represents the term entries in doc index, to be used with InvFPIndex
  i.e., the ID and frequency of all terms in a document.

*/

class InvFPTermList : public TermInfoList {
public:
  InvFPTermList();
  InvFPTermList(int did, int len, vector<LocatedTerm> &tls);
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
  /// Read in a compressed TermInfoList object from a file
  bool binReadC(ifstream& infile);
  /// Write a compressed TermInfoList object to a file
  void binWriteC(ofstream& ofile);

  /// delta decode termids and positions from begin through end
  /// call after read
  virtual void deltaDecode();
  /// delta encode termids and positions from begin through end
  /// call before write
  virtual void deltaEncode();

  /// Change our default sequence of words representation to be bag of words
  void countTerms();

private:

  DOCID_T uid; // this doc's id
  int length;  // length of this document (terms + stopwords)
  LocatedTerm* list; // list of terms and locations
  LLTerm* listcounted; // list of terms and location lists
  int listlen; // number of items we have in list  (same as number of terms)
  int index;   // index for iterator
  int* counts; // keep track of counts of terms for bag of word
  InvFPTerm entry;
  vector<int> loclist; //list of locations to return 

};




#endif
