
#ifndef _TFIDFMODEL_H
#define _TFIDFMODEL_H

/// TF-IDF retrieval model

/*!

  TFIDFModel implements a simple, but empirically effective TFIDF model
with several TF weighting methods. The document-query similarity is
computed using dot product. Three different TF functions are supported
( raw TF, log(TF), BM25 TF). Using BM25 TF formula often leads to a
reasonably good baseline retrieval performance. Noting that the average
document  length needs to be set. (A retrieval evaluation program
would be able to set it to the true average doc length in the collection.)
 
*/

//---------------------------------------------------------------
// CLASSES: TFIDFModel (C. Zhai, 12/1/2000)
//
// TFIDFModel: A TF IDF retrieval model 

// Three different TF functions: raw TF, log(TF), BM25 TF
// 
// Possible use: executing a query on a database
//
//---------------------------------------------------------------

#include "Index.hpp"
#include "WorkingSet.hpp"
#include "IndexedReal.hpp"
#include <math.h>

class TFIDFModel  {
public:

  enum TFMethod  {RAWTF=0, LOGTF=1, BM25=2};
  struct WeightParam {
    WeightParam(TFMethod method= BM25, double k1=1, double b=0.5, double avgLen=500) :
      tf(method), bm25K1(k1), bm25B(b), bm25AvgLen(avgLen) {
    }
    TFMethod tf;
    double bm25K1;
    double bm25B;
    double bm25AvgLen;
  };
 
  static WeightParam defaultQTF;
  static WeightParam defaultDTF;


  /// Perform Rocchio pseudo feedback
  /*!
    input: 
    <PRE>
    query: the weighted unexpanded query vector
    topScores: previous retrieval results
    dbIndex: the index
    docparam: document term weighting parameter
    howManyDocs: how many documents to use for pseudo feedback
    howManyTerms: how many terms to use for query expansion
    posCoeff: the coefficient for positive terms in Rocchio
    </PRE>
    
    output: a modified query vector (stored in query)

    An example of use:

    <PRE>

    //  initial retrieval
    retrieve(query, .., results, .....);
    // feedback 
    computeFeedbackQuery(query, results, ...);
    // retrieval with expanded query
    retrieve(query, ... finalresults, ...);
    </PRE>
    
   */

  static void computeFeedbackQuery(double *query, 
				   IndexedRealVector * topScores,
				   Index &dbIndex,
				   const WeightParam &docparam,
				   int howManyDocs,
				   int howManyTerms,
				   double posCoeff);

  /// perform retrieval over the whole database
  /*!
    Execute a query:
    
    input:
    <PRE>
    query: query terms
    querySum: query length (only useful when isWeightedQuery=false)
    dbIndex: index
    qryparam, docparam: term weighting parameters
    isWeightedQuery: indicating whether query is already weighted
    </PRE>
    output: ranked document IDs with scores (stored in scores).
    
    Side effect: query becomes a weighted vector, even if it was not originally weighted. (This is for convenience with feedback.)

   */

  static void retrieve(double *query,
		       double querySum,
		       Index & dbIndex,
		       IndexedRealVector &scores,
		       const WeightParam &qryparam=defaultQTF,
		       const WeightParam &docparam=defaultDTF,
		       bool isWeightedQuery=false);



  /// Perform retrieval over a working set
  /*!
    input:
    <PRE>
    query: query terms
    querySum: query length (only useful when isWeightedQuery=false)
    workSet: a subset of documents to score
    dbIndex: index
    qryparam, docparam: term weighting parameters
    isWeightedQuery: indicating whether query is already weighted
    </PRE>
    output: ranked document IDs with scores (stored in scores).
    
    Side effect: query becomes a weighted vector, even if it was not originally weighted. (This is for convenience with feedback.)
  */

  static void retrieveWorkSet(double *query,
			      double querySum,
			      WorkingSet &workSet,
			      Index & dbIndex,
			      IndexedRealVector &scores,
			      const WeightParam &qryparam=defaultQTF,
			      const WeightParam &docparam=defaultDTF,
			      bool isWeightedQuery=false);

private:
  static double DocTFWeight(const double rawTF, const int docID, 
			    Index &dbIndex, const WeightParam &param);
  static double QueryTFWeight(const double rawTF, const double qryLength, 
			      const WeightParam &param);
  static double BM25TF(const double rawTF, const double k1, const double b, 
		       const double docLen, const double avgDocLen);
  static double idf(double totalCount, double thisTermCount);

};

inline double TFIDFModel::BM25TF(const double rawTF, const double k1, const double b, 
		     const double docLen, const  double avgDocLen)
{
  return (k1*rawTF/(rawTF+k1*(1-b+b*docLen/avgDocLen)));
}

inline double TFIDFModel::idf(double totalCount, double thisTermCount)
{
  return (log((totalCount+1)/(thisTermCount+0.5)));
}
#endif //_TFIDFMODEL_H


  


