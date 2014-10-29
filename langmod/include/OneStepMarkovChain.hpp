/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _ONESTEPMARKOVCHAIN_HPP
#define _ONESTEPMARKOVCHAIN_HPP


#include "Index.hpp"
#include "WeightedIDSet.hpp"
namespace lemur 
{
  namespace langmod
  {
    
    /// \brief One step markov chain translation model, not fully tested yet
    /*!
      This code is included mainly to illustrate how the toolkit can be used
      for estimating such models. 
    */
    class OneStepMarkovChain {
    public:
      /// construct a MC restricted to a subset of docs
      OneStepMarkovChain(const lemur::utility::WeightedIDSet &docSet, const lemur::api::Index &homeIndex, double *wdNorm, double stopProbAlpha);
      ~OneStepMarkovChain();
  
      /// @name Iteration support
      /*! 
        Given a word, this provides a way to access the translation probability from
        any word to this word. E.g., given w, each iteration would give
        us t(w|w1), t(w|w2), ..., t(w|w_n).
      */
      //@{
      /// Start iteration over w's for t(toWord |w)
      void startFromWordIteration(lemur::api::TERMID_T toWord) const;
      bool hasMoreFromWord() const { 
        while (itPos <= ind.termCountUnique() && fromWordPr[itPos]==0) 
          itPos++;
        return (itPos<= ind.termCountUnique());
      }
      /// Fetch the next word and the prob of "translating" it to the toWord
      void nextFromWordProb(lemur::api::TERMID_T &fromWord, double &prob) const ;
      //@}

    private:
      void computeFromWordProb(lemur::api::TERMID_T toWord) const;
      const lemur::api::Index &ind;
      double alpha;
      int *dSet;
      double *norm;
      mutable double *fromWordPr;
      mutable lemur::api::TERMID_T itPos;
      mutable lemur::api::TERMID_T curToWord;
    };
  }
}


#endif /* _ONESTEPMARKOVCHAIN_HPP */
