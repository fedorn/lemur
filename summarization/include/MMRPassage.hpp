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

#ifndef _MMRPASSAGE_HPP
#define _MMRPASSAGE_HPP

/*!
  An implementation of the abstract class <code>Passage</code> meant to work in conjunction with the MMR summarizer <code>MMRSumm</code>

*/

class MMRPassage : public Passage {

public:
  double wt;
  double wtSim; 
  double qSim;
  double maxSim;

  MMRPassage(const char* id) {
    psg = new passageVec;
    docID = (char*) id;
    score = -1;
    marked = 0;
    wt = 1.0;
    qSim = 0.0;
    wtSim = 0.0;
    maxSim = -1.0;
  }

  virtual void clear();

  virtual void addTerm(termCount term);

  virtual void addTerms(passageVec pv);

  virtual passageVec* getAsVector(void);

  virtual int operator<(Passage &b);

  virtual double computeMMR(double lambda) const;

  virtual double dotProduct(MMRPassage b) const;

  virtual void scale(double val) const;

};

#endif
