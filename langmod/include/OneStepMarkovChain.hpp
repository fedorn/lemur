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
  OneStepMarkovChain(const WeightedIDSet &docSet, const Index &homeIndex, double *wdNorm, double stopProbAlpha);
  ~OneStepMarkovChain();
  
  /// @name Iteration support
  /*! 
    Given a word, this provides a way to access the translation probability from
    any word to this word. E.g., given w, each iteration would give
    us t(w|w1), t(w|w2), ..., t(w|w_n).
   */
  //@{
  /// Start iteration over w's for t(toWord |w)
  void startFromWordIteration(int toWord) const;
  bool hasMoreFromWord() const { 
    while (itPos <= ind.termCountUnique() && fromWordPr[itPos]==0) 
      itPos++;
    return (itPos<= ind.termCountUnique());
  }
  /// Fetch the next word and the prob of "translating" it to the toWord
  void nextFromWordProb(int &fromWord, double &prob) const ;
  //@}

private:
  void computeFromWordProb(int toWord) const;
  const Index &ind;
  double alpha;
  int *dSet;
  double *norm;
  mutable double *fromWordPr;
  mutable int itPos;
  mutable int curToWord;
};


#endif /* _ONESTEPMARKOVCHAIN_HPP */
