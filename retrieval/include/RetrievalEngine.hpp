#ifndef _RETRIEVALENGINE_HPP
#define _RETRIEVALENGINE_HPP

#include "RetrievalMethod.hpp"

/// A generic retrieval engine

class RetrievalEngine {
public:
  RetrievalEngine(RetrievalMethod &retMethod);
  ~RetrievalEngine();

  /// Performs simple retrieval and returns a ranked list of documents with scores
  virtual void retrieve(TextQuery &qry, IndexedRealVector * &results);

  /// Performs pseudo-feedback retrieval and returns a ranked list of documents with scores
  virtual void retrievePseudoFeedback(TextQuery &qry, int howManyDoc, IndexedRealVector *&results);

  /// a general scoring procedure shared by many different models
  virtual void scoreInvertedIndex(QueryRep &qryRep, IndexedRealVector &sortedScores);

protected:


  Index *ind; // handle of index
  double *scAcc; // score accumulators
  int *status; // scoring status
  IndexedRealVector *res; // sorted doc scores
  QueryRep *qryRep;
  DocumentRep *docRep;
  RetrievalMethod &method;
};


inline RetrievalEngine::~RetrievalEngine()
{
  delete [] scAcc;
  delete [] status;
  delete res;
}

#endif /* _RETRIEVALENGINE_HPP */

