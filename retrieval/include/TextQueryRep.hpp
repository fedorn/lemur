/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _TEXTQUERYREP_HPP
#define _TEXTQUERYREP_HPP

#include "RetrievalMethod.hpp"
#include "TextQuery.hpp"
#include "IndexedReal.hpp"
#include "Index.hpp"
#include "Counter.hpp"
#include "WeightedIDSet.hpp"
#include "FreqVector.hpp"

namespace lemur 
{
  namespace api 
  {
    
    //------------------------------------------------------------
    //      Abstract Interface for Text Query Representation 
    //------------------------------------------------------------

    /// A query term is assumed to have at least an ID and a weight

    class QueryTerm {
    public:
      QueryTerm(TERMID_T termID, double weight) : ti(termID), w(weight) {
      } 
      virtual ~QueryTerm() {}
      virtual TERMID_T id() const { return ti;}
      virtual double weight() const { return w;}
    protected:
      TERMID_T ti;
      double w;
    };

    /// Abstract representation of a text query as a sequence of weighted terms

    class TextQueryRep : public QueryRep {
    public:
      virtual ~TextQueryRep() {}
      /// This, along with hasMore(), nextTerm(), supports iteration over terms
      virtual void startIteration() const = 0;
      virtual bool hasMore() const = 0;

      /// Fetch the next term. A new instance is generated; the caller is responsible for deleting it!
      virtual QueryTerm *nextTerm() const = 0;

      /// Any query-specific constant term in the scoring formula
      virtual double scoreConstant() const = 0;

    };
  }
}
namespace lemur 
{
  namespace retrieval 
  {

    /// Representation of a query with a double array 

    class ArrayQueryRep : public lemur::api::TextQueryRep {
    public:
      /// The size passed in must be large enough to hold all the terms
      ArrayQueryRep(int size) : ct(new lemur::utility::ArrayCounter<double>(size)), scConst(0) {
      }
      /// The size passed in must be large enough to hold all the terms, typically dbIndex.termCountUnique()+1.
      ArrayQueryRep(int size, const lemur::api::TermQuery &qry, const lemur::api::Index &dbIndex);
      /// build a query rep with a frequency vector
      ArrayQueryRep(int size, const lemur::utility::FreqVector &qryVec);

      virtual ~ArrayQueryRep() { delete ct; }


      virtual void startIteration() const {
        ct->startIteration();
      }

      virtual bool hasMore() const{
        return (ct->hasMore());
      }


      virtual lemur::api::QueryTerm *nextTerm() const;


      virtual void incCount(lemur::api::TERMID_T wdIndex, double count) {
        ct->incCount(wdIndex,count);
      }

      virtual void setCount(lemur::api::TERMID_T wdIndex, double count) {
        ct->setCount(wdIndex,count);
      }

      virtual double totalCount() const { return ct->sum();}

      virtual double scoreConstant() const{ return scConst;}

      virtual void setScoreConstant(double scoreConst) { scConst = scoreConst;}


    protected:
      virtual lemur::api::QueryTerm *makeQueryTerm(lemur::api::TERMID_T wdIndex, double wdCount) const {
        return (new lemur::api::QueryTerm(wdIndex, wdCount));
      }
      double scConst;
      lemur::utility::ArrayCounter<double> *ct;
    };
  }
}
namespace lemur 
{
  namespace api
  {
    
    /// Representation of a subset of feedback documents

    class PseudoFBDocs : public DocIDSet {
    public:
      /// howManyDocs < 0 means using all results. If ignoreWeight=true, each doc will have the same weight (1.0); otherwise the weight comes from "results".
      PseudoFBDocs(IndexedRealVector &results, int howManyDoc, bool ignoreWeight=true):
        res(&results), howMany(howManyDoc), noWeight(ignoreWeight) {
      }
      virtual ~PseudoFBDocs() {}

      virtual void startIteration() const{
        it = res->begin();
        i=0;
      }  
      virtual bool hasMore() const;

      virtual void nextIDInfo(int &id, double &relProb) const;

    private:
      mutable IndexedRealVector *res;
      int howMany;
      mutable int i;
      bool noWeight;
      mutable IndexedRealVector::iterator it;
    };
  }
}
#endif /* _TEXTQUERYREP_HPP */
