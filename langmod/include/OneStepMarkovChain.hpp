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

/// One step markov chain translation model, not fully tested yet
/*!
  This code is included mainly to illustrate how the toolkit can be used
for estimating such models. 
 */

#include "Index.hpp"
#include "WeightedIDSet.hpp"

class OneStepMarkovChain {
public:
  /// construct a MC restricted to a subset of docs
  OneStepMarkovChain(WeightedIDSet &docSet, Index &homeIndex, double stopProbAlpha);
  ~OneStepMarkovChain();
  
  /// @name Iteration support
  /*! 
    Given a word, this provides a way to access the translation probability from
    any word to this word. E.g., given w, each iteration would give
    us t(w|w1), t(w|w2), ..., t(w|w_n).
   */
  //@{
  /// Start iteration over w's for t(toWord |w)
  void startFromWordIteration(int toWord);
  bool hasMoreFromWord() { 
    while (itPos <= ind.termCountUnique() && fromWordPr[itPos]==0) 
      itPos++;
    return (itPos<= ind.termCountUnique());
  }
  /// Fetch the next word and the prob of "translating" it to the toWord
  void nextFromWordProb(int &fromWord, double &prob);
  //@}

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
