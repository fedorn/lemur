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


#ifndef _RETRIEVALMETHOD_HPP
#define _RETRIEVALMETHOD_HPP


#include "Index.hpp"
#include "WeightedIDSet.hpp"
#include "IndexedReal.hpp"

namespace lemur 
{
  namespace api
  {

    //------------------------------------------------------------
    //      Abstract Interface for A Query
    //------------------------------------------------------------

    /// Abstract query
    class Query {
    public:
      virtual ~Query() {}

      virtual const string& id() const { return qid; }
      virtual void id(const string& str) { qid = str; }

    protected:
      string qid;
    };

    /// Abstract query representation

    class QueryRep {
    public:
      virtual ~QueryRep(){}
    };


    //------------------------------------------------------------
    //      Abstract Interface for Feedback Document Subset
    //------------------------------------------------------------

    /// Reuse a WeightedIDSet to represent A feedback doc set, with the weight interpreted as the corresponding "prob. of relevance" which could be any score that may be useful for feedback
    typedef lemur::utility::WeightedIDSet DocIDSet;


    //------------------------------------------------------------
    //      Abstract Interface for A Retrieval Method/Model
    //------------------------------------------------------------



    /*!
      A retrieval method is determined by specifying the following elements <BR>
      <UL>
      <LI> A method to compute the query representation
      <LI> A method to score a document w.r.t. a query representation
      <LI> A method to score all the documents in the collection w.r.t. a query representation
      <LI> A method to update the query representation based on a set of (relevant) documents
      </UL>
    */

    class RetrievalMethod {
    public:
      RetrievalMethod(const Index &collectionIndex) : ind(collectionIndex) {}
      virtual ~RetrievalMethod() {}

      /// compute the representation for a query, semantics defined by subclass
      virtual QueryRep *computeQueryRep(const Query &qry)=0;

      /// Score a document identified by the id w.r.t. a query rep
      virtual double scoreDoc(const QueryRep &qry, DOCID_T docID)=0;

      /// Score a set of documents w.r.t. a query rep (e.g. for re-ranking)
      /*! 
        The default implementation provided by this class is to call function scoreDoc
      */
      virtual void scoreDocSet(const QueryRep &qry, const DocIDSet &docSet, IndexedRealVector &results);
  
      /// Score all documents in the collection
      /*! 
        The default implementation provided by this class is to call 
        function scoreDoc, thus to score documents one by one. 
        This is inefficient, so usually a subclass should override this method
        if a more efficient scoring, e.g., based on inverted index, is possible.
      */

      virtual void scoreCollection(const QueryRep &qry, IndexedRealVector &results);

      /// update the query, feedback support
      virtual void updateQuery (QueryRep &qryRep, const DocIDSet &relDocs) = 0;

    protected:
      const Index &ind;
    };
  }
}

#endif /* _RETRIEVALMETHOD_HPP */
