/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  Author: dmf 9/2002
 */

#ifndef _COSSIMRETMETHOD_HPP
#define _COSSIMRETMETHOD_HPP

#include "TextQueryRetMethod.hpp"

/// Parameters used in the CosSim retrieval method
namespace CosSimParameter {
  struct FeedbackParam {
    int howManyTerms;
    double posCoeff;
  };    
  static int defaultHowManyTerms = 50;
  static double defaultPosCoeff = 0.5;
  static const char *defaultL2File = "cos.L2";
};

/// Representation of a query (as a weighted vector) in the CosSim method
class CosSimQueryRep : public ArrayQueryRep {
public:
  /// Create a query representation for a text query.
  CosSimQueryRep(TextQuery &qry, Index &dbIndex, double *idfValue);
  /// Create a query representation for the given document id.
  CosSimQueryRep(int docId, Index &dbIndex, double *idfValue);
  virtual ~CosSimQueryRep() {}
protected:
  double *idf;
  Index &ind;
};

/// Representation of a doc (as a weighted vector) in the CosSim method.
class CosSimDocRep : public DocumentRep {
public:
  CosSimDocRep(int docID, Index &dbIndex, double *idfValue, double norm) : 
    DocumentRep(docID), ind(dbIndex), idf(idfValue), dNorm(norm) {
  }
  virtual ~CosSimDocRep() { }
  /// return dtf*idf.
  virtual double termWeight(int termID, DocInfo *info) { 
    return (idf[termID]*info->termCount()); 
  }
  /// return the L2 norm for this document.
  virtual double scoreConstant() { return dNorm;}

private:
  Index & ind;
  double *idf;
  /// L2 norm for this document.
  double dNorm; 
};

/// The ScoreFunction for the CosSim method.
class CosSimScoreFunc : public ScoreFunction {
public:
  CosSimScoreFunc(Index &dbIndex): ind(dbIndex) {}
  /// score adjustment (divide by L2 Norm for document)
  virtual double adjustedScore(double origScore, TextQueryRep *qRep, 
			       DocumentRep *dRep) {
    return origScore/dRep->scoreConstant();
  }
protected:
  Index &ind;
};

/// Cosine similarity retrieval method.
class CosSimRetMethod : public TextQueryRetMethod {
public:
  /// Precomputes the idf values for the collection.
  CosSimRetMethod(Index &dbIndex, ScoreAccumulator &accumulator);
  /// Precomputes the idf values for the collection. Loads the
  /// precomputed L2 document norms from L2File (if present).
  /// This support file is optional, if it is not found, L2 norms
  /// will be computed during scoring and cached across queries.
  CosSimRetMethod(Index &dbIndex, const char *L2file,
		  ScoreAccumulator &accumulator);

  virtual ~CosSimRetMethod();

  virtual TextQueryRep *computeTextQueryRep(TextQuery &qry) {
    return (new CosSimQueryRep(qry, ind, idfV));
  }

  virtual TextQueryRep *computeTextQueryRep(int docid) {
    return (new CosSimQueryRep(docid, ind, idfV));
  }

  virtual DocumentRep *computeDocRep(int docID) { 
    return (new CosSimDocRep(docID, ind, idfV, docNorm(docID)));
  }
  virtual ScoreFunction *scoreFunc() {
    return (scFunc);
  }
  virtual void updateTextQuery(TextQueryRep &qryRep, DocIDSet &relDocs);
  void setFeedbackParam(CosSimParameter::FeedbackParam &feedbackParam);
protected:
  
  double *idfV;
  ScoreFunction *scFunc;
  double * docNorms;
  int numDocs, numTerms;

  /// @name Parameters
  //@{
  CosSimParameter::FeedbackParam fbParam;
  /// Name of support file containing the precomputed L2 norms.
  const char *L2FileName;
  //@}

  /// compute the L2 norm for a given docID.
  double docNorm(int docID);
  /// load the precomputed L2 norms support file.
  void loadDocNorms();
};

inline void CosSimRetMethod::setFeedbackParam(CosSimParameter::FeedbackParam 
					      &feedbackParam) {
  fbParam = feedbackParam;
}
#endif /* _COSSIMRETMETHOD_HPP */
