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



#ifndef _TFIDFRETMETHOD_HPP
#define _TFIDFRETMETHOD_HPP

#include "TextQueryRetMethod.hpp"

/// Parameters used in the TFIDF retrieval method
namespace TFIDFParameter {

  enum TFMethod  {RAWTF=0, LOGTF=1, BM25=2};
  struct WeightParam {
    TFMethod tf;
    double bm25K1;
    double bm25B;
  };
  struct FeedbackParam {
    int howManyTerms;
    double posCoeff;
  };    
  static double defaultDocK1=1;
  static double defaultDocB = 0.5;
  static double defaultQryK1 = 1;
  static double defaultQryB = 0;
  static int defaultHowManyTerms = 50;
  static double defaultPosCoeff = 0.5;
};

/// Representation of a query (as a weighted vector) in the TFIDF method
class TFIDFQueryRep : public ArrayQueryRep {
public:
  TFIDFQueryRep(const TermQuery &qry, const Index &dbIndex, double *idfValue, TFIDFParameter::WeightParam &param);

  virtual ~TFIDFQueryRep() {}

  double queryTFWeight(const double rawTF) const;
protected:
  TFIDFParameter::WeightParam &prm;
  double *idf;
  const Index &ind;
};

/// Representation of a doc (as a weighted vector) in the TFIDF method
class TFIDFDocRep : public DocumentRep {
public:
  TFIDFDocRep(int docID, const Index &dbIndex, double *idfValue,
	      TFIDFParameter::WeightParam &param) : 
    DocumentRep(docID), ind(dbIndex), prm(param), idf(idfValue) {
  }
  virtual ~TFIDFDocRep() { }
  virtual double termWeight(int termID, const DocInfo *info) const{ 
    return (idf[termID]*docTFWeight(info->termCount())); 
  }
  virtual double scoreConstant() const { return 0;}

  double docTFWeight(const double rawTF) const;
private:

  const Index & ind;
  TFIDFParameter::WeightParam &prm;
  double *idf;
};


/// The TFIDF retrieval method with a few TF formula options

class TFIDFRetMethod : public TextQueryRetMethod {
public:

  TFIDFRetMethod(const Index &dbIndex, ScoreAccumulator &accumulator);
  virtual ~TFIDFRetMethod() {delete [] idfV; delete scFunc;}

  virtual TextQueryRep *computeTextQueryRep(const TermQuery &qry) {
    return (new TFIDFQueryRep(qry, ind, idfV, qryTFParam));
  }

  virtual DocumentRep *computeDocRep(int docID) { 
    return (new TFIDFDocRep(docID, ind, idfV, docTFParam));
  }
  virtual ScoreFunction *scoreFunc() {
    return (scFunc);
  }


  virtual void updateTextQuery(TextQueryRep &qryRep, const DocIDSet &relDocs);

  void setDocTFParam(TFIDFParameter::WeightParam &docTFWeightParam);

  void setQueryTFParam(TFIDFParameter::WeightParam &queryTFWeightParam);

  void setFeedbackParam(TFIDFParameter::FeedbackParam &feedbackParam);

  static double BM25TF(const double rawTF, const double k1, const double b, 
		       const double docLen, const double avgDocLen);

protected:
  double *idfV;
  ScoreFunction *scFunc;
  
  /// @name Parameters
  //@{

  TFIDFParameter::WeightParam qryTFParam;
  TFIDFParameter::WeightParam docTFParam;
  TFIDFParameter::FeedbackParam fbParam;

  //@}

};


inline void TFIDFRetMethod::setDocTFParam(TFIDFParameter::WeightParam &docTFWeightParam)
{
  docTFParam = docTFWeightParam;
}



inline void TFIDFRetMethod::setQueryTFParam(TFIDFParameter::WeightParam &queryTFWeightParam)
{
  qryTFParam = queryTFWeightParam;
}


inline void TFIDFRetMethod::setFeedbackParam(TFIDFParameter::FeedbackParam &feedbackParam)
{
  fbParam = feedbackParam;
}



inline double TFIDFRetMethod ::BM25TF(const double rawTF, const double k1, const double b, 
		     const double docLen, const  double avgDocLen)
{
  double x= rawTF+k1*(1-b+b*docLen/avgDocLen);
  return (k1*rawTF/x);
}



#endif /* _TFIDFRETMETHOD_HPP */







