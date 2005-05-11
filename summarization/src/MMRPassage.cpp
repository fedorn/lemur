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

#include "MMRPassage.hpp"
#include "Passage.hpp"

void lemur::summarization::MMRPassage::clear() {
  psg.erase(psg.begin(), psg.end());
}  

int lemur::summarization::MMRPassage::operator<(const Passage &b) const{
  const MMRPassage* mPsg = dynamic_cast<const MMRPassage*>(&b);
  return (score < mPsg->score);
}

void lemur::summarization::MMRPassage::addTerm(termCount term) {
  // :TODO: set counts right
  psg.push_back(term);
}

void lemur::summarization::MMRPassage::addTerms(const passageVec pv) {
  // :TODO: set counts right
  for (int i=0; i<pv.size(); i++) {
    psg.push_back(pv[i]);
  }
}

const lemur::summarization::passageVec* 
lemur::summarization::MMRPassage::getAsVector(void) const {
  return &psg;
}

double lemur::summarization::MMRPassage::computeMMR(double lambda) const {
  return lambda*wtSim - (1-lambda)*maxSim;
}

double lemur::summarization::MMRPassage::dotProduct(MMRPassage b) const {
  double retVal = 0.0;
  for(int i=0; i<psg.size(); i++) {
    for(int j=0; j<b.psg.size(); j++) {
      if ( psg[i].termID == b.psg[j].termID ) {
        retVal += psg[i].val * b.psg[j].val;
      }
    }
  } 

  return retVal;
}

// This is NOT const, it modifies psg.
//void lemur::summarization::MMRPassage::scale(double val) const {
void lemur::summarization::MMRPassage::scale(double val)  {
  passageVec::iterator it;
  it = psg.begin();
  while (it < psg.end()) {
    (*it).val = (*it).val * val;
  }
}
