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


#ifndef _TEXTQUERYREP_HPP
#define _TEXTQUERYREP_HPP

#include "RetrievalMethod.hpp"
#include "Document.hpp"
#include "IndexedReal.hpp"
#include "Index.hpp"
#include "Counter.hpp"
#include "WeightedIDSet.hpp"
#include "FreqVector.hpp"

//------------------------------------------------------------
//      Abstract Interface for Text Query 
//------------------------------------------------------------

/// A text query is an adaptor of Document
class TextQuery : public Query {
public:
  TextQuery(Document &doc) : d(doc) {}
  virtual ~TextQuery() {}

  virtual char *id() { return d.getID();}
  virtual void startTermIteration() { d.startTermIteration();}
  virtual bool hasMore() { return d.hasMore();}
  virtual TokenTerm *nextTerm() { return d.nextTerm();}
protected:
  Document &d;
};



//------------------------------------------------------------
//      Abstract Interface for Text Query Representation 
//------------------------------------------------------------

/// A query term is assumed to have at least an ID and a weight

class QueryTerm {
public:
  QueryTerm(int termID, double weight) : ti(termID), w(weight) {
  } 
  ~QueryTerm() {}
  virtual int id() { return ti;}
  virtual double weight() { return w;}
protected:
  int ti;
  double w;
};

/// Abstract representation of a text query as a sequence of weighted terms

class TextQueryRep : public QueryRep {
public:
  virtual ~TextQueryRep() {}
  /// This, along with hasMore(), nextTerm(), supports iteration over terms
  virtual void startIteration() = 0;
  virtual bool hasMore() = 0;

  /// Fetch the next term. A new instance is generated; the caller is responsible for deleting it!
  virtual QueryTerm *nextTerm() = 0;

  /// Any query-specific constant term in the scoring formula
  virtual double scoreConstant() = 0;

};




/// Representation of a query with a double array 

class ArrayQueryRep : public TextQueryRep {
public:
  /// The size passed in must be large enough to hold all the terms
  ArrayQueryRep(int size) : ct(new ArrayCounter<double>(size)), scConst(0) {
  }
  /// The size passed in must be large enough to hold all the terms, typically dbIndex.termCountUnique()+1.
  ArrayQueryRep(int size, TextQuery &qry, Index &dbIndex);
  /// build a query rep with a frequency vector
  ArrayQueryRep(int size, FreqVector &qryVec);

  virtual ~ArrayQueryRep() { delete ct; }


  virtual void startIteration() {
    ct->startIteration();
  }

  virtual bool hasMore() {
    return (ct->hasMore());
  }


  virtual QueryTerm *nextTerm();


  virtual void incCount(int wdIndex, double count) {
    ct->incCount(wdIndex,count);
  }

  virtual void setCount(int wdIndex, double count) {
    ct->setCount(wdIndex,count);
  }

  virtual double totalCount() { return ct->sum();}

  virtual double scoreConstant() { return scConst;}

  virtual void setScoreConstant(double scoreConst) { scConst = scoreConst;}


protected:
  virtual QueryTerm *makeQueryTerm(int wdIndex, double wdCount) {
    return (new QueryTerm(wdIndex, wdCount));
  }
  double scConst;
  ArrayCounter<double> *ct;
};

/// Representation of a subset of feedback documents

class PseudoFBDocs : public DocIDSet {
public:
  /// howManyDocs < 0 means using all results. If ignoreWeight=true, each doc will have the same weight (1.0); otherwise the weight comes from "results".
  PseudoFBDocs(IndexedRealVector &results, int howManyDoc, bool ignoreWeight=true):
    res(&results), howMany(howManyDoc), noWeight(ignoreWeight) {
  }
  virtual ~PseudoFBDocs() {}

  virtual void startIteration() {
    it = res->begin();
    i=0;
  }  
  virtual bool hasMore() ;

  virtual void nextIDInfo(int &id, double &relProb);

private:
  IndexedRealVector *res;
  int howMany;
  int i;
  bool noWeight;
  IndexedRealVector::iterator it;
};



#endif /* _TEXTQUERYREP_HPP */





