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

#ifndef _INVFPTERM_HPP
#define _INVFPTERM_HPP

#include "TermInfoList.hpp"
#include "InvFPTypes.hpp"

/// term class for InvIndex and InvFPIndex
/*! 
*/

class InvFPTerm: public TermInfo {
public:
  friend class InvTermList;
  friend class InvFPTermList;

  InvFPTerm() { freq=1;};
  ~InvFPTerm() {};

  const char* spelling() { return word; };
  void spelling(TERM_T term) { word = term; };
  int strLength() { return strlen; };
  void strLength(int len) { strlen = len; };
  int count() {return freq; };
  TERMID_T id() { return tid; };
  LOC_T position() { return loc; };
  vector<LOC_T>* positions() { return loclist; };
  void position(LOC_T pos) { loc = pos; };

protected:

  TERMID_T tid;
  int strlen;
  TERM_T word; 
  int freq; // number of times this term occurs in this documen
  LOC_T loc;  // where this term (currently) occurs in the document
  vector<LOC_T>* loclist; // list of all places term occurs in the document                  
};

#endif
