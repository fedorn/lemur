#ifndef _UNIGRAMLM_HPP
#define _UNIGRAMLM_HPP

#include "Counter.hpp"
#include "Exception.hpp"
#include <string.h>
/// Abstract Unigram Language Model class
/*!
  The index range ([0,N]) is assumed to be defined by the lexicon.
  The caller is responsible for boundary check if necessary. 
  Any undefined index would have a zero probability. 
 */
class UnigramLM {
public:
  /// return p(w) 
  virtual double prob(int wordIndex) = 0;
  /// return a string ID of the lexicon on which the word index should be interpreted
  virtual const char * lexiconID() = 0;

  /// iteration over non-zero probability entries
  virtual void startIteration() = 0;
  virtual bool hasMore() = 0;
  virtual void nextWordProb(int &wordIndex, double &prob) = 0;
};


/// Common implementation of a (smoothed) unigram LM estimated based on a counter

class SmoothedMLEstimator : public UnigramLM {
public:
  SmoothedMLEstimator(Counter &counter, const char *lexiconID) : ct(counter), lexID(lexiconID) {}
  virtual ~SmoothedMLEstimator() {}

  virtual double prob(int wordIndex) {
    return (probEstimate(wordIndex, ct.count(wordIndex),ct.sum()));
  }

  virtual void startIteration() {
    ct.startIteration();
  }

  virtual bool hasMore() {
    ct.hasMore();
  }

  virtual void nextWordProb(int &wordIndex, double &prob) {
    double count;
    ct.nextCount(wordIndex, count);
    prob = probEstimate(wordIndex, count, ct.sum());
  }
  
  virtual const char * lexiconID() { return lexID;}

  /// individual model differs in its implementation of probEstimate() method
  virtual double probEstimate(int wordIndex, double wdCount, double sumCount) =0;

protected:
  Counter &ct;
  const char *lexID;
};
  
/// Maximum Likelihood Estimator

class MLUnigramLM : public SmoothedMLEstimator { 
public:
  MLUnigramLM(Counter & counter, const char *lexiconID) : SmoothedMLEstimator(counter, lexiconID) {};
  virtual ~MLUnigramLM() {}
  
  virtual double probEstimate(int wordIndex, double count, double sum) {
    return (count/sum);
  }
};

/// Laplace-smoothed unigram language model
class LaplaceUnigramLM : public SmoothedMLEstimator { 
public:
  LaplaceUnigramLM(Counter & counter, const char *lexiconID, double vocabSize) : SmoothedMLEstimator(counter, lexiconID), vocSz(vocabSize) {};
  virtual ~LaplaceUnigramLM() {}
  
  virtual double probEstimate(int wordIndex, double count, double sum) {
    return ((count+1)/(sum+vocSz));
  }
private:
  double vocSz;
};


/// Dirichlet prior smoothing

class DirichletUnigramLM : public SmoothedMLEstimator { 
public:
  DirichletUnigramLM(Counter & counter, const char *lexiconID, 
		     UnigramLM &refLM, double priorSampleSize) 
    : SmoothedMLEstimator(counter, lexiconID), ref(&refLM), 
    s(priorSampleSize) {}

  virtual ~DirichletUnigramLM() {}
  
  virtual double probEstimate(int wordIndex, double count, double sum) {
    return ((count+s*ref->prob(wordIndex))/(sum+s));
  }

private:
  UnigramLM *ref;
  /// prior sample size
  double s;  
};


/// Linear interpolation

class InterpUnigramLM : public UnigramLM {
public:
  InterpUnigramLM(UnigramLM &langMod1, UnigramLM &langMod2, double coeff1) : 
    lm1(langMod1), lm2(langMod2), c1(coeff1) { 
    if (strcmp(lm1.lexiconID(), lm2.lexiconID())) {
      throw Exception("InterpUnigramLM", "non-compatible lexicon");
      // ensure compatible lexicons
    }
  }

  virtual ~InterpUnigramLM() {}
  
  virtual double prob(int wordIndex) {
    return (c1*lm1.prob(wordIndex)+(1-c1)*lm2.prob(wordIndex));
  }

  virtual const char *lexiconID() {
    return lm1.lexiconID();
  }

private:
  UnigramLM &lm1;
  UnigramLM &lm2;
  double c1;  
};





#endif /* _UNIGRAMLM_HPP */
