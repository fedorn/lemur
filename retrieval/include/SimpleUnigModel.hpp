#ifndef _SIMPLEUNIGMODEL_HPP
#define _SIMPLEUNIGMODEL_HPP



/*!

 \file

\brief Query likelihood unigram retrieval model with different smoothing methods

 Class SimpleUnigramModel: A simple unigram language model retrieval function
                  Evaluating P(q|d) by smoothing the ML estimator of P(q|d)
                  with collection language model
 
 Class SimpleDocUnigramModel: Abstract interface of any smoothed document
                           language model

 JelinekMercerDocModel, BayesianDocModel, AbsoluteDiscountDocModel: 
       Specific smoothing methods.

 Possible use: evaluating language model smoothing for retrieval


 An example of use:

 <PRE>
 
 double *queryArray;
 ...
 // read in queryArray and querySum
 
 double *collectPr;
 
 // set collectPr (collection language model)

 SimpleDocUnigramModel *docModel;

 // instantiate a specific smoothing method, e.g., absolute discounting
 
 docModel = new AbsoluteDiscountDocModel(...);

 // finally, call the retrieval function to execute the query
 SimpleUnigramModel::Retrieve(queryArray,
                              querySum,
                              collectPr,
			      ...
			      *docModel ...);


</PRE>
*/


//---------------------------------------------------------------
// CLASSES: SimpleUnigramModel (C. Zhai, 12/18/2000)
//
// SimpleUnigramModel: A simple unigram language model retrieval function
//                  Evaluating P(q|d) by smoothing the ML estimator of P(q|d)
//                  with collection language model
// 
// classes: SimpleDocUnigramModel
//
//    SimpleDocUnigramModel: Abstract interface of any smoothed document
//                           language model
// Possible use: evaluating language model smoothing, etc.
//
//---------------------------------------------------------------

#include "Index.hpp"
#include "IndexedReal.hpp"

/// abstract interface of a smoothed document unigram model
/*!
 abstract interface of a smoothed document unigram model

 <PRE>
 p(w|d) = q(w|d) if w seen
        = a(d) * Pc(w)  if w unseen
   where, a(d) is the sum of probability mass allocated to all unseen words
          Pc(w) is the collection language model
</PRE>

*/

class SimpleDocUnigramModel {
public:
  virtual ~SimpleDocUnigramModel () {}
  /// a(d)
  virtual double unseenCoeff(int docID)=0; // a(d)
  /// p(w|d), w seen
  virtual double seenProb(int termFreq, int termID, int docID)=0; // p(w|d), w seen
};

/// Jelinek-Mercer interpolation 

/*!

<PRE>
 P(w|d) = lambda*Pml(w|d)+(1-lambda)*Pc(w)
</PRE>
*/

class JelinekMercerDocModel : public SimpleDocUnigramModel {
public:
  JelinekMercerDocModel(Index *referenceIndex, 
			double *collectProb,
			double *docProbMass,
			double maxLikeWeight, 
			bool backoffVersion=false): 
    refIndex(referenceIndex),
    collectPr(collectProb),
    docPrMass(docProbMass),
    lambda(maxLikeWeight), 
    backoff(backoffVersion) {
  };

  virtual ~JelinekMercerDocModel() {};

  virtual double unseenCoeff(int docID) {
    return (backoff? 
	    (1-lambda)/(1-docPrMass[docID]):
	    1-lambda);
  }
  virtual double seenProb(int termFreq, int termID, int docID) {
    if (backoff) {
      return (lambda*termFreq/(double)refIndex->docLength(docID));
    } else {
      return (lambda*termFreq/(double)refIndex->docLength(docID)+
	      (1-lambda)*collectPr[termID]);
    }
  }
private:
  Index *refIndex;
  double *collectPr;
  double *docPrMass;
  double lambda;
  bool backoff;
};

/// Bayesian smoothing with Dirichlet prior
/*!
<PRE>
 P(w|d) = (c(w;d)+lambda*Pc(w))/(|d|+lambda)
</PRE>
*/
class BayesianDocModel : public SimpleDocUnigramModel {
public:
  BayesianDocModel(Index *referenceIndex, 
		   double *collectProb,
		   double *docProbMass,
		   double priorWordCount, 
		   bool backoffVersion=false): 
    refIndex(referenceIndex),
    collectPr(collectProb),
    docPrMass(docProbMass),
    lambda(priorWordCount),
    backoff(backoffVersion) {
  };

  virtual ~BayesianDocModel() {};

  virtual double unseenCoeff(int docID) {
    if (backoff) {
      return (lambda/((lambda+refIndex->docLength(docID))*
		      (1-docPrMass[docID])));
    } else {
      return lambda/(lambda+refIndex->docLength(docID));
    }
  }

  virtual double seenProb(int termFreq, int termID, int docID) {
    if (backoff) {
      return (termFreq/
	      (double)(refIndex->docLength(docID)+lambda));
    } else {      
      
      return (termFreq+lambda*collectPr[termID])/
	(double)(refIndex->docLength(docID)+lambda);
    }
  }
private:
  Index *refIndex;
  double *collectPr;
  double *docPrMass;
  double lambda;
  bool backoff;
};

/// Absolute discout smoothing

/*!
 P(w|d) = (termFreq - delta)/|d|+ lambda*Pc(w)     if seen
      or = lambda*Pc(w) if unseen
 where, lambda =  unique-term-count-in-d*delta/|d|
*/

class AbsoluteDiscountDocModel : public SimpleDocUnigramModel {
public:
  AbsoluteDiscountDocModel(Index *referenceIndex, 
			   double *collectProb,
			   double *docProbMass,
			   int *uniqueTermCount,
			   double discount,
			   bool backoffVersion=false): 
    refIndex(referenceIndex),
    collectPr(collectProb),
    docPrMass(docProbMass),
    uniqDocLen(uniqueTermCount),
    delta(discount),
    backoff(backoffVersion) {
  };

  virtual ~AbsoluteDiscountDocModel() {};
  
  virtual double unseenCoeff(int docID) {
    if (backoff) {
      return (delta*uniqDocLen[docID]/
	      (refIndex->docLength(docID)*(1-docPrMass[docID])));
    } else {
      return (delta*uniqDocLen[docID]/(double)refIndex->docLength(docID));
    } 
  }
  virtual double seenProb(int termFreq, int termID, int docID) {
    if (backoff) {
      return ((termFreq-delta)/(double)refIndex->docLength(docID));
    } else {
      return ((termFreq-delta)/(double)refIndex->docLength(docID)+
	      delta*uniqDocLen[docID]*collectPr[termID]/
	      (double)refIndex->docLength(docID));
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

class SimpleUnigramModel  {
public:

  static void Retrieve(double *query,
		       double querySum,
		       double *collectPr,
		       Index & dbIndex,
		       IndexedRealVector &scores,
		       SimpleDocUnigramModel &docModel,
		       double queryFidelity);
};




#endif //_SIMPLEUNIGRAMMODEL_H


  


