#ifndef _QUERYREP_HPP
#define _QUERYREP_HPP

#include "Document.hpp"
#include "IndexedReal.hpp"
#include "Index.hpp"
#include "Counter.hpp"
#include "WeightedIDSet.hpp"

//------------------------------------------------------------
//      Abstract Interface for Text Query 
//------------------------------------------------------------

/// A text query is just a document
typedef Document TextQuery;


//------------------------------------------------------------
//      Abstract Interface for Feedback Document Subset
//------------------------------------------------------------

/// Reuse a WeightedIDSet to represent A feedback doc set, with the weight interpreted as the corresponding "prob. of relevance" which could be any score that may be useful for feedback
typedef WeightedIDSet DocIDSet;

//------------------------------------------------------------
//      Abstract Interface for Query Representation 
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

/// Abstract representation of a query as a sequence of weighted terms

class QueryRep {
public:
  virtual ~QueryRep() {}
  /// This, along with hasMore(), nextTerm(), supports iteration over terms
  virtual void startIteration() = 0;
  virtual bool hasMore() = 0;
  virtual QueryTerm *nextTerm() = 0;

  /// Any query-specific constant term in the scoring formula
  virtual double scoreConstant() = 0;

};




/// Representation of a query with a double array 

class ArrayQueryRep : public QueryRep {
public:
  ArrayQueryRep(int size) : ct(new ArrayCounter<double>(size)), scConst(0) {
  }
  ArrayQueryRep(int size, TextQuery &qry, Index &dbIndex);
  
  virtual ~ArrayQueryRep() { delete ct; }


  virtual void startIteration() {
    ct->startIteration();
  }

  virtual bool hasMore() {
    return (ct->hasMore());
  }


  virtual QueryTerm *nextTerm() {
    int wdIndex;
    double wdCount;
    ct->nextCount(wdIndex, wdCount);
    return (makeQueryTerm(wdIndex, wdCount));
  }

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


class PseudoFBDocs : public DocIDSet {
public:
  PseudoFBDocs(IndexedRealVector &results, int howManyDoc):
    res(&results), howMany(howManyDoc) {
  }
  ~PseudoFBDocs() {}

  virtual void startIteration() {
    it = res->begin();
    i=0;
  }  
  virtual bool hasMore() {
    return (it != res->end() && i < howMany);
  }

  virtual void nextIDInfo(int &id, double &relProb) {
    id = (*it).ind;
    relProb = (*it).val;
    it++;
    i++;
  }

private:
  IndexedRealVector *res;
  int howMany;
  int i;
  IndexedRealVector::iterator it;
};



#endif /* _QUERYREP_HPP */





