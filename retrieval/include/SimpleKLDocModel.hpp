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
			bool backoffVersion=false): 
    SimpleKLDocModel(docID, collectLM),
    refIndex(referenceIndex),
    docPrMass(docProbMass),
    lambda(collectLMWeight), 
    backoff(backoffVersion) {
  };

  virtual ~JelinekMercerDocModel() {};
  
  virtual double unseenCoeff() {
    return (backoff? 
	    lambda/(1-docPrMass[id]):
	    lambda);
  }
  virtual double seenProb(double termFreq, int termID) {
    if (backoff) {
      return ((1-lambda)*termFreq/(double)refIndex->docLength(id));
    } else {
      return ((1-lambda)*termFreq/(double)refIndex->docLength(id)+
	      lambda*refLM.prob(termID));
    }
  }
private:
  Index *refIndex;
  double *docPrMass;
  double lambda;
  bool backoff;
};

/// Bayesian smoothing with Dirichlet prior
/*!
<PRE>
 P(w|d) = (c(w;d)+mu*Pc(w))/(|d|+mu)
</PRE>
*/
class BayesianDocModel : public SimpleKLDocModel {
public:
  BayesianDocModel(int docID,
		   Index *referenceIndex, 
		   UnigramLM &collectLM,
		   double *docProbMass,
		   double priorWordCount, 
		   bool backoffVersion=false): 
    SimpleKLDocModel(docID, collectLM),
    refIndex(referenceIndex),
    docPrMass(docProbMass),
    mu(priorWordCount),
    backoff(backoffVersion) {
  };

  virtual ~BayesianDocModel() {};

  virtual double unseenCoeff() {
    if (backoff) {
      return (mu/((mu+refIndex->docLength(id))*
		      (1-docPrMass[id])));
    } else {
      return mu/(mu+refIndex->docLength(id));
    }
  }

  virtual double seenProb(double termFreq, int termID) {
    if (backoff) {
      return (termFreq/
	      (double)(refIndex->docLength(id)+mu));
    } else {      
      
      return (termFreq+mu*refLM.prob(termID))/
	(double)(refIndex->docLength(id)+mu);
    }
  }
private:
  Index *refIndex;
  double *docPrMass;
  double mu;
  bool backoff;
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
			   bool backoffVersion=false): 
    SimpleKLDocModel(docID, collectLM),
    refIndex(referenceIndex),
    docPrMass(docProbMass),
    uniqDocLen(uniqueTermCount),
    delta(discount),
    backoff(backoffVersion) {
  };

  virtual ~AbsoluteDiscountDocModel() {};
  
  virtual double unseenCoeff() {
    if (backoff) {
      return (delta*uniqDocLen[id]/
	      (refIndex->docLength(id)*(1-docPrMass[id])));
    } else {
      return (delta*uniqDocLen[id]/(double)refIndex->docLength(id));
    } 
  }
  virtual double seenProb(double termFreq, int termID) {
    if (backoff) {
      return ((termFreq-delta)/(double)refIndex->docLength(id));
    } else {
      return ((termFreq-delta)/(double)refIndex->docLength(id)+
	      delta*uniqDocLen[id]*refLM.prob(termID)/
	      (double)refIndex->docLength(id));
    }
  }
private:
  Index *refIndex;
  double *collectPr;
  double *docPrMass;
  int *uniqDocLen;
  double delta;
  bool backoff;
};


#endif /* _SIMPLEKLDOCMODEL_HPP */





