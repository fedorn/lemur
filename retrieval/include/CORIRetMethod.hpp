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
#include <iostream>

///The default tffactor tfbase values for the collection selection case and the document retrieval case
#define CSTFBASELINE 50
#define CSTFFACTOR 150
#define DOCTFBASELINE 0.5
#define DOCTFFACTOR 1.5
#define MINBELIEF 0.4

class CORIQueryRep : public ArrayQueryRep {
public:
  CORIQueryRep(const TermQuery & qry, const Index & dbIndex);
  virtual ~CORIQueryRep() {}

protected:
  const Index & ind;
};

class CORIDocRep : public DocumentRep {
public:
  CORIDocRep(int docID, const Index & dbIndex, double * cwRatio, 
	     double TFfact = 150, double TFbase = 50, 
	     const SimpleKLDocModel * smoother = NULL,
	     const UnigramLM * collectLM = NULL);
  virtual ~CORIDocRep() { }
  virtual double termWeight(int termID, const DocInfo * info) const ;

  virtual double scoreConstant() const { return 0; }

private:

  const Index & ind;

  int * cwCounts;

  const SimpleKLDocModel * dfSmooth;
  const UnigramLM * collLM;

  double c05;
  double idiv;
  double tnorm;
};



class CORIRetMethod : public TextQueryRetMethod {
public:

  CORIRetMethod(const Index & dbIndex, ScoreAccumulator &accumulator, 
		   String cwName, int isCSIndex=0,
		const SimpleKLDocModel ** smoothers = NULL, 
		const UnigramLM * collectLM = NULL);
  ~CORIRetMethod() { delete scFunc; delete [] cwRatio; }

  virtual TextQueryRep * computeTextQueryRep(const TermQuery & qry) {
    return new CORIQueryRep(qry, ind);
  }
  virtual DocumentRep * computeDocRep(int docID) { 
    if (dfSmooth != NULL) {
      return new CORIDocRep(docID, ind, cwRatio, tffactor, tfbaseline, dfSmooth[docID], collLM);
    }
    return new CORIDocRep(docID, ind, cwRatio, tffactor, tfbaseline);
  }
  virtual ScoreFunction * scoreFunc() {
    return scFunc;
  }

  virtual void scoreCollection(const QueryRep &qry, IndexedRealVector &results);

  virtual void updateTextQuery(TextQueryRep &qryRep, const DocIDSet &relDocs) { }
  
  void setTFFactor(double tf) { tffactor = tf; }
  void setTFBaseline(double tf) { tfbaseline = tf; }

protected:

  ScoreFunction * scFunc;
  const SimpleKLDocModel ** dfSmooth;
  const UnigramLM * collLM;

  double * cwRatio;
  double tffactor;
  double tfbaseline;
  
};

class CORIScoreFunc : public ScoreFunction {
public:
  CORIScoreFunc(const Index & index) : ind(index) {
    rmax=0;
    double dc = ind.docCount();
    c05 = dc + 0.5;
    idiv = log(dc + 1);
    //    qr=NULL;
    first=0;
  }

  virtual double adjustedScore(double origScore, const TextQueryRep * qRep,
			       const DocumentRep * dRep) const {
    /*
    if (qr != qRep) {
      qr = qRep;
      
      qRep->startIteration();
      rmax = 0;
      double qw = 0;
      while (qRep->hasMore()) {
	int qtid = qRep->nextTerm()->id();
	rmax += (1-MINBELIEF)*(log(c05 / ind.docCount(qtid)) / idiv);
      }
    }
    if ((origScore/rmax)>=1){
      cout<<"!!!!!!!!!"<<endl;
      cout<<origScore<<" "<<rmax<<" "<<(origScore / rmax)<<endl;
      }*/
    //return (origScore / rmax);
    return origScore;
  }

private:
  const Index & ind;
  int first;
  //TextQueryRep * qr;
  double rmax;
  double c05;
  double idiv;
};


#endif /* _CORIRETMETHOD_HPP */
