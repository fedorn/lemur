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


#ifndef _SIMPLEKLDOCMODEL_HPP
#define _SIMPLEKLDOCMODEL_HPP

#include "DocumentRep.hpp"
#include "Index.hpp"
#include "UnigramLM.hpp"


/// Simple KL divergence retrieval model parameters
namespace SimpleKLParameter {
  enum SmoothMethod  {JELINEKMERCER=0, DIRICHLETPRIOR=1, ABSOLUTEDISCOUNT=2};
 
  enum SmoothStrategy  {INTERPOLATE=0, BACKOFF=1};

  enum QueryUpdateMethod {MIXTURE = 0, DIVMIN=1, MARKOVCHAIN=2};

  struct DocSmoothParam {
    /// smoothing method
    enum SmoothMethod smthMethod;
    /// smoothing strategy
    enum SmoothStrategy smthStrategy;
    /// discount constant (delta) in absolute discounting
    double ADDelta;
    /// collection model coefficient (lambda) in Jelinek-Mercer
    double JMLambda;
    /// prior (mu) in Dirichlet prior
    double DirPrior;
  };

  static enum SmoothMethod defaultSmoothMethod = DIRICHLETPRIOR;
  static enum SmoothStrategy defaultSmoothStrategy = INTERPOLATE;
  static double defaultADDelta = 0.7;
  static double defaultJMLambda = 0.5;
  static double defaultDirPrior = 1000;

  struct QueryModelParam {
    /// query model re-estimation/updating method
    enum QueryUpdateMethod fbMethod;
    /// Q_new = (1-fbCoeff)*Q_old + fbCoeff*FBModel
    double fbCoeff;
    /// how many terms to use for the re-estimated query model
    int fbTermCount;
    /// feedback query model term probability threshold (only terms with a higher prob. will be used
    double fbPrTh;
    /// feedback query model prob. sum threshold (taking terms up to the point, where the accumulated prob. mass exceeds the threshold
    double fbPrSumTh;
    /// collection influence coefficient (e.g., in mixture model and divergence minimization methods)
    double fbMixtureNoise;
    //// max iterations for EM algorithm (will stop earlier if the likelihood converges with an error smaller than 0.5)
    int emIterations;
  };

  static enum QueryUpdateMethod defaultFBMethod = MIXTURE;
  static double defaultFBCoeff = 0.5;
  static int defaultFBTermCount =50;
  static double defaultFBPrTh = 0.001;
  static double defaultFBPrSumTh = 1;
  static double defaultFBMixNoise = 0.5;
  static int defaultEMIterations = 50;

};


/// Doc representation for simple KL divergence retrieval model

/*!
 abstract interface of doc representation for smoothed document unigram model
 
 adapt a smoothed document language model interface to a DocumentRep interface
 <PRE>
 p(w|d) = q(w|d) if w seen
        = a(d) * Pc(w)  if w unseen
   where,  a(d) controls the probability mass allocated to all unseen words and     Pc(w) is the collection language model
</PRE>

*/

class SimpleKLDocModel : public DocumentRep {
public:
  SimpleKLDocModel(int docID, UnigramLM &collectLM) : DocumentRep(docID), refLM(collectLM) {}
  ~SimpleKLDocModel() {};

  /// term weighting function, weight(w) = p_seen(w)/p_unseen(w)
  virtual double termWeight(int termID, DocInfo *info) {
    return (seenProb(info->termCount(), termID)/(unseenCoeff()* refLM.prob(termID)));
  }

  /// doc-specific constant term in the scoring formula
  virtual double scoreConstant() {
    return unseenCoeff();
  }

  /// a(d)
  virtual double unseenCoeff()=0; // a(d)
  /// p(w|d), w seen
  virtual double seenProb(double termFreq, int termID)=0; // p(w|d), w seen
protected:
  UnigramLM &refLM;
};



/// Jelinek-Mercer interpolation 

/*!

<PRE>
 P(w|d) = (1-lambda)*Pml(w|d)+ lambda*Pc(w)
</PRE>
*/

class JelinekMercerDocModel : public SimpleKLDocModel {
public:
  JelinekMercerDocModel(int docID, 
			Index *referenceIndex, 
			UnigramLM &collectLM,
			double *docProbMass,
			double collectLMWeight, 
			SimpleKLParameter::SmoothStrategy smthStrategy=SimpleKLParameter::INTERPOLATE): 
    SimpleKLDocModel(docID, collectLM),
    refIndex(referenceIndex),
    docPrMass(docProbMass),
    lambda(collectLMWeight), 
    strategy(smthStrategy) {
  };

  virtual ~JelinekMercerDocModel() {};
  
  virtual double unseenCoeff() {
    if (strategy == SimpleKLParameter::INTERPOLATE) {
      return lambda;
    } else if (strategy==SimpleKLParameter::BACKOFF) {
      return lambda/(1-docPrMass[id]);
    } else {
      throw Exception("JelinekMercerDocModel", "Unknown smoothing strategy");
    }
  }
  virtual double seenProb(double termFreq, int termID) {
    if (strategy == SimpleKLParameter::INTERPOLATE) {
      return ((1-lambda)*termFreq/(double)refIndex->docLength(id)+
	      lambda*refLM.prob(termID));
    } else if (strategy == SimpleKLParameter::BACKOFF) {
      return ((1-lambda)*termFreq/(double)refIndex->docLength(id));
    } else {
      throw Exception("JelinekMercerDocModel", "Unknown smoothing strategy");
    }
  }
private:
  Index *refIndex;
  double *docPrMass;
  double lambda;
  SimpleKLParameter::SmoothStrategy strategy;
};

/// Bayesian smoothing with Dirichlet prior
/*!
<PRE>
 P(w|d) = (c(w;d)+mu*Pc(w))/(|d|+mu)
</PRE>
*/
class DirichletPriorDocModel : public SimpleKLDocModel {
public:
  DirichletPriorDocModel(int docID,
		   Index *referenceIndex, 
		   UnigramLM &collectLM,
		   double *docProbMass,
		   double priorWordCount, 
	    	SimpleKLParameter::SmoothStrategy smthStrategy=SimpleKLParameter::INTERPOLATE): 
    SimpleKLDocModel(docID, collectLM),
    refIndex(referenceIndex),
    docPrMass(docProbMass),
    mu(priorWordCount),
	    strategy(smthStrategy) {
  };

  virtual ~DirichletPriorDocModel() {};

  virtual double unseenCoeff() {

    if (strategy == SimpleKLParameter::INTERPOLATE) {
      return mu/(mu+refIndex->docLength(id));
    } else if (strategy==SimpleKLParameter::BACKOFF) {
      return (mu/((mu+refIndex->docLength(id))*
		      (1-docPrMass[id])));
    } else {
      throw Exception("DirichletPriorDocModel", "Unknown smoothing strategy");
    }
  }

  virtual double seenProb(double termFreq, int termID) {
    if (strategy == SimpleKLParameter::INTERPOLATE) {
      return (termFreq+mu*refLM.prob(termID))/
	(double)(refIndex->docLength(id)+mu);
    } else if (strategy == SimpleKLParameter::BACKOFF) {
      return (termFreq/
	      (double)(refIndex->docLength(id)+mu));
    } else {      
      throw Exception("DirichletPriorDocModel", "Unknown smoothing strategy");
    }
  }
private:
  Index *refIndex;
  double *docPrMass;
  double mu;
  SimpleKLParameter::SmoothStrategy strategy;
};

/// Absolute discout smoothing

/*!
 P(w|d) = (termFreq - delta)/|d|+ lambda*Pc(w)     if seen
      or = lambda*Pc(w) if unseen
 where, lambda =  unique-term-count-in-d*delta/|d|
*/

class AbsoluteDiscountDocModel : public SimpleKLDocModel {
public:
  AbsoluteDiscountDocModel(int docID,
			   Index *referenceIndex, 
			   UnigramLM &collectLM,
			   double *docProbMass,
			   int *uniqueTermCount,
			   double discount,
		SimpleKLParameter::SmoothStrategy smthStrategy=SimpleKLParameter::INTERPOLATE): 
    SimpleKLDocModel(docID, collectLM),
    refIndex(referenceIndex),
    docPrMass(docProbMass),
    uniqDocLen(uniqueTermCount),
    delta(discount),
      strategy(smthStrategy) {
  };

  virtual ~AbsoluteDiscountDocModel() {};
  
  virtual double unseenCoeff() {

    if (strategy == SimpleKLParameter::INTERPOLATE) {
      return (delta*uniqDocLen[id]/(double)refIndex->docLength(id));
    } else if (strategy==SimpleKLParameter::BACKOFF) {
      return (delta*uniqDocLen[id]/
	      (refIndex->docLength(id)*(1-docPrMass[id])));
    } else {
      throw Exception("AbsoluteDiscountDocModel", "Unknown smoothing strategy");
    }
  }
  virtual double seenProb(double termFreq, int termID) {
    if (strategy == SimpleKLParameter::INTERPOLATE) {
      return ((termFreq-delta)/(double)refIndex->docLength(id)+
	      delta*uniqDocLen[id]*refLM.prob(termID)/
	      (double)refIndex->docLength(id));
    } else if (strategy == SimpleKLParameter::BACKOFF) {
      return ((termFreq-delta)/(double)refIndex->docLength(id));
    } else {
            throw Exception("AbsoluteDiscountDocModel", "Unknown smoothing strategy");
    }
  }
private:
  Index *refIndex;
  double *collectPr;
  double *docPrMass;
  int *uniqDocLen;
  double delta;
  SimpleKLParameter::SmoothStrategy strategy;
};


#endif /* _SIMPLEKLDOCMODEL_HPP */





