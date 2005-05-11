/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "BasicPassage.hpp"
#include "Passage.hpp"

void lemur::summarization::BasicPassage::clear() {
  psg.erase(psg.begin(), psg.end());
}  

void lemur::summarization::BasicPassage::addTerms(const passageVec pv) {
  for (int i=0; i<pv.size(); i++) {
    psg.push_back(pv[i]);
  }
}

void lemur::summarization::BasicPassage::addTerm(termCount term) {
  psg.push_back(term);
}
  
const lemur::summarization::passageVec* 
lemur::summarization::BasicPassage::getAsVector(void) const {
  return &psg;
}
  
int lemur::summarization::BasicPassage::operator<(const Passage &b) const {
  const BasicPassage* bPsg = dynamic_cast<const BasicPassage*>(&b);
  return (score < bPsg->score);
}
