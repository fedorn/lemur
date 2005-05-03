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


#ifndef _UNIGRAMLM_HPP
#define _UNIGRAMLM_HPP

#include "Counter.hpp"
#include "Exception.hpp"
#include "IndexTypes.hpp"
#include <cstring>
namespace lemur 
{
  namespace langmod
  {
    
    /// Abstract Unigram Language Model class
    /*!
      The index range ([0,N]) is assumed to be defined by the lexicon.
      The caller is responsible for boundary check if necessary. 
      Any undefined index would have a zero probability. 
    */
    class UnigramLM {
    public:
      /// return p(w) 
      virtual double prob(lemur::api::TERMID_T wordIndex) const = 0;
      /// return a string ID of the lexicon on which the word index should be interpreted
      virtual const string lexiconID() const= 0;

      /// iteration over non-zero probability entries
      virtual void startIteration() const = 0;
      virtual bool hasMore() const = 0;
      virtual void nextWordProb(lemur::api::TERMID_T &wordIndex, double &prob) const = 0;
    };


    /// Common implementation of a (smoothed) unigram LM estimated based on a counter

    class SmoothedMLEstimator : public UnigramLM {
    public:
      SmoothedMLEstimator(const lemur::utility::Counter &counter, const string &lexiconID) : ct(counter), lexID(lexiconID) {}
      virtual ~SmoothedMLEstimator() {}

      virtual double prob(lemur::api::TERMID_T wordIndex) const {
        return (probEstimate(wordIndex, ct.count(wordIndex),ct.sum()));
      }

      virtual void startIteration() const {
        ct.startIteration();
      }

      virtual bool hasMore() const {
        return ct.hasMore();
      }

      virtual void nextWordProb(lemur::api::TERMID_T &wordIndex, double &prob) const{
        double count;
        //dmf FIXME
        ct.nextCount((int&)wordIndex, count);
        prob = probEstimate(wordIndex, count, ct.sum());
      }
  
      virtual const string lexiconID() const { return lexID;}

      /// individual model differs in its implementation of probEstimate() method
      virtual double probEstimate(lemur::api::TERMID_T wordIndex, double wdCount, double sumCount) const=0;

    protected:
      const lemur::utility::Counter &ct;
      const string lexID;
    };
  
    /// Maximum Likelihood Estimator

    class MLUnigramLM : public SmoothedMLEstimator { 
    public:
      MLUnigramLM(const lemur::utility::Counter & counter, const string &lexiconID) : SmoothedMLEstimator(counter, lexiconID) {};
      virtual ~MLUnigramLM() {}
  
      virtual double probEstimate(lemur::api::TERMID_T wordIndex, double count, double sum) const{
        return (count/sum);
      }
    };

    /// Laplace-smoothed unigram language model
    class LaplaceUnigramLM : public SmoothedMLEstimator { 
    public:
      LaplaceUnigramLM(const lemur::utility::Counter & counter, const string &lexiconID, double vocabSize) : SmoothedMLEstimator(counter, lexiconID), vocSz(vocabSize) {};
      virtual ~LaplaceUnigramLM() {}
  
      virtual double probEstimate(lemur::api::TERMID_T wordIndex, double count, double sum) const {
        return ((count+1)/(sum+vocSz));
      }
    private:
      double vocSz;
    };


    /// Dirichlet prior smoothing

    class DirichletUnigramLM : public SmoothedMLEstimator { 
    public:
      DirichletUnigramLM(const lemur::utility::Counter & counter, const string &lexiconID, 
                         const UnigramLM &refLM, double priorSampleSize) 
        : SmoothedMLEstimator(counter, lexiconID), ref(&refLM), 
          s(priorSampleSize) {}

      virtual ~DirichletUnigramLM() {}
  
      virtual double probEstimate(lemur::api::TERMID_T wordIndex, double count, double sum) const {
        return ((count+s*ref->prob(wordIndex))/(sum+s));
      }

    private:
      const UnigramLM *ref;
      /// prior sample size
      double s;  
    };






    /// Linear interpolation smoothing

    class InterpUnigramLM : public SmoothedMLEstimator { 
    public:
      InterpUnigramLM(const lemur::utility::Counter & counter, const string &lexiconID, 
                      const UnigramLM &refLM, double refCoeff) 
        : SmoothedMLEstimator(counter, lexiconID), ref(&refLM), 
          refC(refCoeff) {}

      virtual ~InterpUnigramLM() {}
  
      virtual double probEstimate(lemur::api::TERMID_T wordIndex, double count, double sum) const {
        return ((1-refC)*count/sum + refC*ref->prob(wordIndex));
      }

    private:
      const UnigramLM *ref;
      /// coefficient for the reference language model
      double refC;  
    };
 
  }
}
#endif /* _UNIGRAMLM_HPP */
