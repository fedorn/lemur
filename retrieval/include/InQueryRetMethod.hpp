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
/// Representation of a query in the InQuery method
class InQueryRep : public StructQueryRep {
public:
  InQueryRep(const TermQuery &qry, const Index &dbIndex, double db):
    StructQueryRep(qry, dbIndex, db) {}
  virtual ~InQueryRep() {}
};

/// The InQuery retrieval method for structured queries. Implements the
/// InQuery formulation of idf weighting and default belief value. Behavior
/// can be modified by adjusting that value using the defaultBelief 
/// parameter. Provides feedback support in the same fashion as the LEMUR 
/// TFIDF method.

class InQueryRetMethod : public StructQueryRetMethod {
public:

  //  InQueryRetMethod(Index &dbIndex, ScoreAccumulator &accumulator);
  InQueryRetMethod(const Index &dbIndex, double belief, int fbTerms, 
		   double fbCoef, bool cacheIDF);
  virtual ~InQueryRetMethod() {
    delete[](idfV); 
    delete(scFunc);
  }

  virtual StructQueryRep *computeStructQueryRep(const TermQuery &qry) {
    return (new InQueryRep(qry, ind, defaultBelief));
  }

  virtual DocumentRep *computeDocRep(int docID) { 
    return (new StructQryDocRep(docID, idfV, ind.docLength(docID),
				docCount, docLengthAverage, 
				defaultBelief));
  }
  virtual ScoreFunction *scoreFunc() {
    return (scFunc);
  }
  /// Create new structured query that is a weighted sum of the original
  /// query and the terms selected.
  virtual void updateStructQuery(StructQueryRep &qryRep, const DocIDSet &relDocs);
private:
  double *idfV;
  ScoreFunction *scFunc; 
  double fbCoeff;
  int fbTermCount;
  double defaultBelief;
  int docCount;
  double docLengthAverage;
};
#endif /* _INQUERYRETMETHOD_HPP */







