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


#ifndef _OKAPIRETMETHOD_HPP
#define _OKAPIRETMETHOD_HPP

#include "TextQueryRep.hpp"
#include "ScoreFunction.hpp"
#include "TextQueryRetMethod.hpp"
#include <cmath>

/// Okapi retrieval model parameter namespace

namespace OkapiParameter {
  struct TFParam {
    double k1;
    double b;
    double k3;
  }; 

  static double defaultK1 = 1.2;
  static double defaultB = 0.75;
  static double defaultK3 = 7;
  
  struct FeedbackParam {
    /// The (pseudo) count of a new term added to the query (It's unclear how this is set from the Okapi paper, so it's implemented as a parameter.)
    double expQTF; // expanded query term TF
    /// The number of terms to add for feedback
    int howManyTerms;
  };

  static double defaultExpQTF = 0.5;
  static int defaultHowManyTerms = 50;
};

/// Represent of a query term in Okapi retrieval model, the term carries a count of the number of rel docs with the term

class OkapiQueryTerm : public QueryTerm {
public:
  OkapiQueryTerm(int termID, double count, int pEstCount, double paramK3) : QueryTerm(termID, count), pEst(pEstCount), k3(paramK3) {
  }
  /// return the number of rel docs with the term
  int pEstCount() { return pEst;}

  /// return query term TF weight
  double weight() { 
    return ((k3+1)*w/(k3+w));
  } 
private:
  int pEst;
  double k3;
};

/// The Okapi scoring function

class OkapiScoreFunc : public ScoreFunction {
public:
  OkapiScoreFunc(Index &dbIndex): ind(dbIndex) {}
  virtual double matchedTermWeight(QueryTerm *qTerm, TextQueryRep *qRep, DocInfo *info, DocumentRep *dRep);
protected:
  Index &ind;
};


/// OkapiQueryRep carries an array to store the count of relevant docs with a term 
class OkapiQueryRep : public ArrayQueryRep {
public:
  // initial query constructor, no feedback docs assumed
  OkapiQueryRep(TextQuery &qry, Index &dbIndex, double paramK3);

  virtual ~OkapiQueryRep() { delete [] pEst; }
  /// return total number of relevant/feedback documents
  int pNormCount() { return pNorm;}
  /// set total number of relevant/feedback documents
  void setPNormCount(int count) { pNorm = count;}
  /// increase the count of relevant/feedback doc in which a term occurs
  void incPEst(int wdIndex, int val) { pEst[wdIndex]+=val;}
protected:
  virtual QueryTerm *makeQueryTerm(int wdIndex, double wdCount) {
    return (new OkapiQueryTerm(wdIndex, wdCount, pEst[wdIndex], k3));
  }
  double k3;
  int pNorm;
  int *pEst;
};

/// Doc representation for Okapi model

class OkapiDocRep : public DocumentRep {
public:
  OkapiDocRep(int docID, Index &dbIndex, OkapiParameter::TFParam &param) : DocumentRep(docID), ind(dbIndex),
  prm(param) {
  }
  virtual ~OkapiDocRep() { }
  virtual double termWeight(int termID, DocInfo *info);
  double BM25TF(double rawTF, double docLength);
  virtual double scoreConstant() { return 0;}
protected:
  Index &ind;
  OkapiParameter::TFParam &prm;
};

/// The Okapi BM25 retrieval function, as described in their TREC-3 paper

class OkapiRetMethod : public TextQueryRetMethod  {
public:


  OkapiRetMethod(Index &dbIndex, ScoreAccumulator &accumulator);

  virtual ~OkapiRetMethod() { delete scFunc;}

  virtual TextQueryRep *computeTextQueryRep(TextQuery &qry) {
    return (new OkapiQueryRep(qry, ind, tfParam.k3));
  }

  virtual DocumentRep *computeDocRep(int docID) {
    return (new OkapiDocRep(docID, ind, tfParam));
  }

  virtual ScoreFunction *scoreFunc();
  
  virtual void updateTextQuery(TextQueryRep &origRep, DocIDSet &relDocs);

  void setTFParam(OkapiParameter::TFParam &tfWeightParam);

  void setFeedbackParam(OkapiParameter::FeedbackParam &feedbackParam);

  static double RSJWeight(double r, double R, 

			  double n, double N) {
    return (log ((r+0.5)*(N-n-R+r+0.5)/((R-r+0.5)*(n-r+0.5))));
  }

protected:
  OkapiScoreFunc *scFunc;

  OkapiParameter::TFParam tfParam;
  OkapiParameter::FeedbackParam fbParam;


};


inline void OkapiRetMethod::setTFParam(OkapiParameter::TFParam &tfWeightParam)
{
  tfParam = tfWeightParam;
}

inline void OkapiRetMethod::setFeedbackParam(OkapiParameter::FeedbackParam &feedbackParam)
{
  fbParam = feedbackParam;
}

inline double OkapiDocRep::BM25TF(double rawTF, double docLength) 
{
  return ((prm.k1+1)*rawTF/(rawTF +  prm.k1*(1-prm.b + prm.b*docLength/ind.docLengthAvg())));
}


inline double OkapiDocRep::termWeight(int termID, DocInfo *info)
{
  return BM25TF(info->termCount(), ind.docLength(info->docID()));
} 




inline ScoreFunction *OkapiRetMethod::scoreFunc()
{
  return scFunc;
}

#endif /* _OKAPIRETMETHOD_HPP */





