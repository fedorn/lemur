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
