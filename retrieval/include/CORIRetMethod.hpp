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


#ifndef _CORIRETMETHOD_HPP
#define _CORIRETMETHOD_HPP


#include "UnigramLM.hpp"
#include "SimpleKLDocModel.hpp"
#include "TextQueryRetMethod.hpp"
#include "Param.hpp"
#include <math.h>



class CORIQueryRep : public ArrayQueryRep {
public:
  CORIQueryRep(TextQuery & qry, Index & dbIndex);
  virtual ~CORIQueryRep() {}

protected:
  Index & ind;
};

class CORIDocRep : public DocumentRep {
public:
  CORIDocRep(int docID, Index & dbIndex, double * cwRatio,
	     SimpleKLDocModel * smoother = NULL,
	     UnigramLM * collectLM = NULL);
  virtual ~CORIDocRep() { }
  virtual double termWeight(int termID, DocInfo * info);

  virtual double scoreConstant() { return 0; }

private:

  Index & ind;

  int * cwCounts;

  SimpleKLDocModel * dfSmooth;
  UnigramLM * collLM;

  double c05;
  double idiv;
  double tnorm;
};



class CORIRetMethod : public TextQueryRetMethod {
public:

  CORIRetMethod(Index & dbIndex, ScoreAccumulator &accumulator, 
		String cwName, 
		SimpleKLDocModel ** smoothers = NULL, 
		UnigramLM * collectLM = NULL);
  ~CORIRetMethod() { delete scFunc; delete [] cwRatio; }

  virtual TextQueryRep * computeTextQueryRep(TextQuery & qry) {
    return new CORIQueryRep(qry, ind);
  }
  virtual DocumentRep * computeDocRep(int docID) { 
    if (dfSmooth != NULL) {
      return new CORIDocRep(docID, ind, cwRatio, dfSmooth[docID], collLM);
    }
    return new CORIDocRep(docID, ind, cwRatio);
  }
  virtual ScoreFunction * scoreFunc() {
    return scFunc;
  }

  virtual void scoreCollection(QueryRep &qry, IndexedRealVector &results);

  virtual void updateTextQuery(TextQueryRep &qryRep, DocIDSet &relDocs) { }


protected:

  ScoreFunction * scFunc;
  SimpleKLDocModel ** dfSmooth;
  UnigramLM * collLM;

  double * cwRatio;
  
};

class CORIScoreFunc : public ScoreFunction {
public:
  CORIScoreFunc(Index & index) : ind(index) {
    double dc = ind.docCount();
    c05 = dc + 0.5;
    idiv = log(dc + 1) / 0.6;
   
  }

  virtual double adjustedScore(double origScore, TextQueryRep * qRep,
			       DocumentRep * dRep) {
    if (qr != qRep) {
      qr = qRep;
      
      qRep->startIteration();
      rmax = 0;
      double qw = 0;
      while (qRep->hasMore()) {
	int qtid = qRep->nextTerm()->id();
	
	rmax += (log(c05 / ind.docCount(qtid)) / idiv);
      }
    }
    return (origScore / rmax);
  }

private:
  Index & ind;

  TextQueryRep * qr;
  double rmax;

  double c05;
  double idiv;
};


#endif /* _CORIRETMETHOD_HPP */
