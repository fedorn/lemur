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
#ifndef _INVFPTERM_HPP
#define _INVFPTERM_HPP

#include "TermInfoList.hpp"
#include "InvFPTypes.hpp"

/// term class for InvFPIndex
/*! 
*/

class InvFPTerm: public TermInfo {
public:
  friend class InvFPTermList;

  InvFPTerm() { freq=1;};
  ~InvFPTerm() {};

  TERM_T spelling() { return word; };
  void spelling(TERM_T term) { word = term; };
  int strLength() { return strlen; };
  void strLength(int len) { strlen = len; };
  int count() {return freq; };
  TERMID_T id() { return tid; };
  LOC_T position() { return loc; };
//  LOC_T* positions() { return loclist; };
  void position(LOC_T pos) { loc = pos; };

private:
  LOC_T loc;  // where this term (currently) occurs in the document
 // LOC_T* loclist; // list of all places term occurs in the document                  
  TERMID_T tid;
  int strlen;
  TERM_T word; 
  int freq; // number of times this term occurs in this document
};

#endif
