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

#include "MMRPassage.hpp"
#include "Passage.hpp"

void MMRPassage::clear() {
  psg->erase(psg->begin(), psg->end());
}  

int MMRPassage::operator<(Passage &b) {
  MMRPassage* mPsg = dynamic_cast<MMRPassage*>(&b);
  return (score < mPsg->score);
}

void MMRPassage::addTerm(termCount term) {
  // :TODO: set counts right
  psg->push_back(term);
}

void MMRPassage::addTerms(passageVec pv) {
  // :TODO: set counts right
  for (int i=0; i<pv.size(); i++) {
    psg->push_back(pv[i]);
  }
}

passageVec* MMRPassage::getAsVector(void) {
  return psg;
}

double MMRPassage::computeMMR(double lambda) const {
  return lambda*wtSim - (1-lambda)*maxSim;
}

double MMRPassage::dotProduct(MMRPassage b) const {
  double retVal = 0.0;
  for(int i=0; i<psg->size(); i++) {
    for(int j=0; j<b.psg->size(); j++) {
      //      cout << (*psg)[i].termID << "==" << (*b.psg)[j].termID << "?";
      if ( (*psg)[i].termID == (*b.psg)[j].termID ) {
	//cout << "DP HIT" << (*psg)[i].val << " " << (*b.psg)[j].val << endl;
	retVal += (*psg)[i].val * (*b.psg)[j].val;
      }
    }
  } 

  return retVal;
}

void MMRPassage::scale(double val) const {
  passageVec::iterator it;
  it = psg->begin();
  while (it < psg->end()) {
    (*it).val = (*it).val * val;
  }
}
