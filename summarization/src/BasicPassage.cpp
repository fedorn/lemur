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

#include "BasicPassage.hpp"
#include "Passage.hpp"

void BasicPassage::clear() {
  psg->erase(psg->begin(), psg->end());
}  

void BasicPassage::addTerms(const passageVec pv) {
  for (int i=0; i<pv.size(); i++) {
    psg->push_back(pv[i]);
  }
}

void BasicPassage::addTerm(termCount term) {
  psg->push_back(term);
}
  
const passageVec* BasicPassage::getAsVector(void) const {
  return psg;
}
  
int BasicPassage::operator<(const Passage &b) const {
  const BasicPassage* bPsg = dynamic_cast<const BasicPassage*>(&b);
  return (score < bPsg->score);
}
