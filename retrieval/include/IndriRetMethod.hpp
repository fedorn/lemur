/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
#ifndef _INDRIRETMETHOD_HPP
#define _INDRIRETMETHOD_HPP

#include "LemurIndriIndex.hpp"
#include "TextQueryRep.hpp"
#include "RetrievalMethod.hpp"
namespace indri
{
  namespace api
  {
    class QueryEnvironment;
  }
}

/// Query model representation for the indri query language
class IndriQueryModel : public QueryRep {
public:
  IndriQueryModel(const TermQuery &qry) {
    // collect terms (with operators) into a single string.
    // could also collect real terms for matchinfo use here...

    qry.startTermIteration();
    while (qry.hasMore()) {
      const Term *t = qry.nextTerm();
      realQuery += t->spelling();
      realQuery += " ";
    }
  }
  
  virtual ~IndriQueryModel() {
  }

  const string &getQuery() const {return realQuery;}
  // for update method.
  void updateQuery(const string &newQuery) const { realQuery = newQuery;}

private:
  mutable string realQuery;
};

class IndriRetMethod : public RetrievalMethod {
public:
  // parameters? want to get the rules in certainly...
  IndriRetMethod(const Index &dbIndex);

  /// clean up.
  virtual ~IndriRetMethod();
  /// set parameters
  virtual void setParams(indri::api::Parameters *parms);
  /// set stopword list
  virtual void setStopwords(const string& stopfile) ;
  /// score all of the documents in the collection
  virtual void scoreCollection(const QueryRep &qry, 
			       IndexedRealVector &results);
  /// score all of the documents in the collection
  virtual void scoreCollection(const string &qry, 
			       IndexedRealVector &results);
  /// create an indri query model.
  virtual QueryRep *computeQueryRep(const Query &qry) {
    if (const TermQuery *q = dynamic_cast<const TermQuery *>(&qry))
      return (new IndriQueryModel(*q));
    else LEMUR_THROW(LEMUR_RUNTIME_ERROR, 
		     "IndriRetMethod expects query of type TermQuery");
  } 

  /// Score a set of documents w.r.t. a query
  virtual void scoreDocSet(const QueryRep &qry, const DocIDSet &docSet, 
			   IndexedRealVector &results);

  /// Score a document identified by the id w.r.t. a query rep
  virtual double scoreDoc(const QueryRep &qry, DOCID_T docID);

  /// update the query
  virtual void updateQuery(QueryRep &qryRep, const DocIDSet &relDocs);
private:
  indri::api::QueryEnvironment *env;
  indri::api::Parameters *params;
};

#endif /* _INDRIRETMETHOD_HPP */
