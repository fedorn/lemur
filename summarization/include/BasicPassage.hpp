/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#include <iomanip>
#include "Passage.hpp"
#include <vector>

using std::vector;

#ifndef _BASICPASSAGE_HPP
#define _BASICPASSAGE_HPP

/*!
  A simple implementation of the abstract class <code>Passage</code> meant to work in conjunction with the simple sentence selection summarizer <code>BasicSumm</code>

*/

class BasicPassage : public Passage {

public:
  BasicPassage(const char* id) {
    psg = new passageVec;
    docID = (char *)id;
    score = -1;
    marked = 0;
  } 
 
  virtual void clear() ;

  virtual void addTerm(termCount term) ;

  virtual void addTerms(passageVec pv) ;

  virtual passageVec* getAsVector(void) ;

  virtual int operator<(Passage &b) ;

};

#endif
