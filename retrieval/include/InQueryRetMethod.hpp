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
  Author: fff
 */

#ifndef _INQUERYRETMETHOD_HPP
#define _INQUERYRETMETHOD_HPP

#include "StructQueryRetMethod.hpp"
#include "StructQryDocRep.hpp"

/// Parameters used in the InQuery retrieval method.
namespace InQueryParameter {
  struct docParam {
    /// default belief value (score) for a node.
    double defaultBelief;
  };
  struct qryParam {
    /// Q_new = (1-fbCoeff)*Q_old + fbCoeff*FBModel
    double fbCoeff;
    /// how many terms to use for the re-estimated query model
    int fbTermCount;
  };

  static double defaultFBCoeff = 0.5;
  static int defaultFBTermCount = 50;
  static double defaultBelief = 0.4;
};

/// Representation of a query (as a weighted vector) in the InQuery method
class InQueryRep : public StructQueryRep {
public:
  /// construct a query model based on query text
  InQueryRep(StructQuery &qry, InvFPIndex &dbIndex) :  StructQueryRep(qry, dbIndex, InQueryParameter::defaultBelief), ind(dbIndex) {}
  virtual ~InQueryRep() {}

protected:
  double *idf;
  InvFPIndex &ind;
};


/// The InQuery retrieval method for structured queries. Implements the
/// InQuery formulation of idf weighting and default belief value. Behavior
/// can be modified by adjusting that value using the defaultBelief 
/// parameter. Provides feedback support in the same fashion as the LEMUR 
/// TFIDF method.

class InQueryRetMethod : public StructQueryRetMethod {
public:

  InQueryRetMethod(InvFPIndex &dbIndex, ScoreAccumulator &accumulator);
  virtual ~InQueryRetMethod() {delete [] idfV; delete scFunc;}

  virtual StructQueryRep *computeStructQueryRep(StructQuery &qry) {
    return (new InQueryRep(qry, index));
  }

  virtual DocumentRep *computeDocRep(int docID) { 
    return (new StructQryDocRep(docID, index, idfV));
  }
  virtual ScoreFunction *scoreFunc() {
    return (scFunc);
  }
  /// Create new structured query that is a weighted sum of the original
  /// query and the terms selected by the simplified Rocchio of the
  /// TFIDFRetMethod class.
  virtual void updateStructQuery(StructQueryRep &qryRep, DocIDSet &relDocs);

  void setDocINQParam(InQueryParameter::docParam &dParam);
  void setQueryINQParam(InQueryParameter::qryParam &qParam);

protected:
  double *idfV;
  /// keep a copy to be used at any time
  ScoreFunction *scFunc; 
  InQueryParameter::qryParam qryPrm;
  InQueryParameter::docParam docPrm;

};

inline void InQueryRetMethod::setDocINQParam(InQueryParameter::docParam &dParam)
{
  docPrm = dParam;
}



inline void InQueryRetMethod::setQueryINQParam(InQueryParameter::qryParam &qParam)
{
  qryPrm = qParam;
}

#endif /* _INQUERYRETMETHOD_HPP */







