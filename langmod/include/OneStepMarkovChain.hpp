 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _ONESTEPMARKOVCHAIN_HPP
#define _ONESTEPMARKOVCHAIN_HPP

/// One step markov chain translation model

#include "Index.hpp"
#include "WeightedIDSet.hpp"

class OneStepMarkovChain {
public:
  OneStepMarkovChain(WeightedIDSet &docSet, Index &homeIndex, double stopProb);
  ~OneStepMarkovChain();
  
  void startFromWordIteration(int toWord);
  bool hasMoreFromWord() { 
    while (itPos <= ind.termCountUnique() && fromWordPr[itPos]==0) 
      itPos++;
    return (itPos<= ind.termCountUnique());
  }
  void nextFromWordProb(int &fromWord, double &prob);
  double *wordNorm() { return norm;}

private:
  void computeFromWordProb(int toWord);
  Index &ind;
  double alpha;
  int *dSet;
  double *norm;
  double *fromWordPr;
  int itPos;
  int curToWord;
};


#endif /* _ONESTEPMARKOVCHAIN_HPP */
