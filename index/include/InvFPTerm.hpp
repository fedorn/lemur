/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#ifndef _INVFPTERM_HPP
#define _INVFPTERM_HPP

#include "TermInfoList.hpp"
#include "Term.hpp"
#include "InvFPTypes.hpp"

/// term class for InvIndex and InvFPIndex
/*! 
*/

class InvFPTerm: public TermInfo, public Term {
public:
  InvFPTerm() { tcount=1;}
  ~InvFPTerm() {};

  int strLength() const { return strlen; }
  void strLength(int len) { strlen = len; }
  LOC_T position() const { return loc; }
  const LOC_T* positions() const {vector<LOC_T>::const_iterator it(loclist->begin()); return &(*it); }
  void position(LOC_T pos) { loc = pos; }
  void positions(vector<LOC_T>* list) { loclist = list; }
protected:
  int strlen;
  LOC_T loc;  // where this term (currently) occurs in the document
  vector<LOC_T>* loclist; // list of all places term occurs in the document                  
};

#endif
