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

class File;

class InvFPTermList : public TermInfoList {
public:
  InvFPTermList();
  InvFPTermList(int did, int len, vector<LocatedTerm> &tls);
  ~InvFPTermList();

  /// prepare iteration
  void startIteration() const;

  /// has more entries
  bool hasMore() const;

  /// Get a pointer to the next entry (pointer to a local static memory)
  TermInfo *nextEntry() const;

  /// Get the length of this document
  int docLength() const{ return length; }

  /// Get the number of terms in this document
  int termCount()  const{ return listlen; }

  /// Get the docid this list is for
  int docID()  const{ return uid; }

  /// Read in a TermInfoList object from a file
  /// Assumes the following format: DOCID DOCLENGTH UNIQUE_TERM_COUNT [TERM LOC]..[]
  bool binRead(ifstream& infile);
  /// Read in a compressed TermInfoList object from a file
  bool binReadC(ifstream& infile);
  /// Write a compressed TermInfoList object to a file
  void binWriteC(ofstream& ofile);

  bool binReadC( File& infile );
  void binWriteC( File& outfile );

  /// delta decode termids and positions from begin through end
  /// call after read
  virtual void deltaDecode();
  /// delta encode termids and positions from begin through end
  /// call before write
  virtual void deltaEncode();

  /// Change our default sequence of words representation to be bag of words
  void countTerms();

protected:
  // Helper functions for iterator, subclasses should override
  /// create new element of this list type for the iterator
  virtual TermInfo* newElement() const { return new InvFPTerm(); }
  /// set element from position, returns pointer to the element
  virtual TermInfo* getElement(TermInfo* elem, POS_T position) const;
  /// copy values from one Element (of type InvFPTerm) to another
  virtual void assignElement(TermInfo* to, TermInfo* from) const {
    *dynamic_cast<InvFPTerm*>(to) = *dynamic_cast<InvFPTerm*>(from);
  }
  /// position at beginning of list
  virtual POS_T beginPosition() const { return (POS_T) 0; }
  /// position at end of list
  virtual POS_T endPosition() const { return (POS_T) listlen; }
  /// advance position
  virtual POS_T nextPosition(POS_T position) const;

  DOCID_T uid; // this doc's id
  int length;  // length of this document (terms + stopwords)
  LocatedTerm* list; // list of terms and locations
  LLTerm* listcounted; // list of terms and location lists
  int listlen; // number of items we have in list  (same as number of terms)
  mutable int index;   // index for iterator
  int* counts; // keep track of counts of terms for bag of word
  mutable InvFPTerm entry;
  mutable vector<int> loclist; //list of locations to return

};




#endif
