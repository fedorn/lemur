/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
  Author: fff
*/


#ifndef _STRUCTQUERYRETMETHOD_HPP
#define _STRUCTQUERYRETMETHOD_HPP

#include "RetrievalMethod.hpp"
#include "StructQueryRep.hpp"
#include "DocumentRep.hpp"
#include "ScoreFunction.hpp"
#include "ScoreAccumulator.hpp"
#include "FreqVector.hpp"

namespace lemur 
{
  namespace api
  {
    
    /// \brief Abstract Interface for A Retrieval Method/Model for Structured Query
    /*!
      Given a query <tt>q = op(q<sub>1</sub>,q<sub>2</sub>,...,q<sub>N</sub>)</tt> 
      where q<sub>i</sub> can be an structured query node or a term node,
      and a document <tt>d</tt>, <tt>StructQueryRetMethod</tt> assumes the 
      following general scoring function:

      <pre>
      s(q,d) = g(s(q<sub>1</sub>,d), ..., s(q<sub>N</sub>,d))
      </pre>

      That is, the score of a document <tt>d</tt> against a query <tt>q</tt> is a 
      function <tt>g</tt> of the scores of each child query node in 
      <tt>q<sub>i</sub></tt>, where <tt>g</tt> is the composition function for 
      <tt>q</tt>, such as average.
 
    */

    class StructQueryRetMethod : public RetrievalMethod {
    public:
      StructQueryRetMethod(const Index &ind) : RetrievalMethod(ind) {}
      virtual ~StructQueryRetMethod() {}
      /// compute the query representation for a text query 
      //(caller responsible for deleting the memory of the generated new instance)
      virtual lemur::retrieval::StructQueryRep *computeStructQueryRep(const TermQuery &qry) = 0;

      /// overriding abstract class method
      virtual QueryRep *computeQueryRep(const Query &qry); 
      /// score the query against the given document id.
      virtual double scoreDoc(const QueryRep &qry, DOCID_T docID);
      /// score the query against the collection.
      virtual void scoreCollection(const QueryRep &qry, IndexedRealVector &results);

      /// compute the doc representation (caller responsible for deleting 
      /// the memory of the generated new instance)
      virtual DocumentRep *computeDocRep(DOCID_T docID) = 0;
      /// return the scoring function pointer
      virtual ScoreFunction *scoreFunc() = 0;
      /// update the query
      virtual void updateQuery(QueryRep &qryRep, const DocIDSet &relDocs) {
        updateStructQuery(*((lemur::retrieval::StructQueryRep *)(&qryRep)), relDocs);
      }

      /// Modify/update the query representation based on a set (presumably)
      /// relevant documents
      virtual void updateStructQuery(lemur::retrieval::StructQueryRep &qryRep, 
                                     const DocIDSet &relDocs) = 0;

      /// Scoring with the inverted index
      virtual void scoreInvertedIndex(const QueryRep &qryRep, 
                                      IndexedRealVector &scores, 
                                      bool scoreAll=false);
    };

    /// ScoreFunction for structured querys. Provides evalQuery.
    class StructQueryScoreFunc : public ScoreFunction {
    public:
      /// Have the QueryNode eval itself on the DocumentRep. 
      virtual double evalQuery(const lemur::retrieval::QueryNode *qn, const DocumentRep *dRep) {
        return qn->eval(dRep);
      }
      /// No adjustment, return the original score.
      virtual double adjustedScore(double w, const lemur::retrieval::QueryNode *qn) {
        return w;
      }  
    };

    //=============== inlines ========================================

    inline QueryRep *StructQueryRetMethod::computeQueryRep(const Query &qry) { 
      if (const TermQuery *q = dynamic_cast<const TermQuery *>(&qry))
        return (computeStructQueryRep(*q));
      else LEMUR_THROW(LEMUR_RUNTIME_ERROR,"StructQueryRetMethod expects query of type TermQuery");
    }
  }
}

#endif /* _STRUCTQUERYRETMETHOD_HPP */
