 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _RETRIEVALENGINE_HPP
#define _RETRIEVALENGINE_HPP

#include "RetrievalMethod.hpp"

/// A generic retrieval engine

class RetrievalEngine {
public:
  RetrievalEngine(RetrievalMethod &retMethod);
  ~RetrievalEngine();

  /// Performs simple retrieval and returns a ranked list of documents with scores (The reference pointer "results" will point to the ranked list inside the engine, so the user doesn't need to allocate memory for "results") 
  virtual void retrieve(TextQuery &qry, IndexedRealVector * &results);

  /// Performs pseudo-feedback retrieval and returns a ranked list of documents  with scores (The reference pointer "results" will point to the ranked list inside the engine, so the user doesn't need to allocate memory for "results") 
  virtual void retrievePseudoFeedback(TextQuery &qry, int howManyDoc, IndexedRealVector *&results);

  /// a general scoring procedure shared by many different models (assuming "sortedScores has memory allocated)
  virtual void scoreInvertedIndex(QueryRep &qryRep, IndexedRealVector &sortedScores);

protected:


  Index *ind; // handle of index
  double *scAcc; // score accumulators
  int *status; // scoring status
  IndexedRealVector *res; // sorted doc scores
  QueryRep *qryRep; // query representation variable
  DocumentRep *docRep; // doc representation variable
  RetrievalMethod &method; //retrieval method handle
};


inline RetrievalEngine::~RetrievalEngine()
{
  delete [] scAcc;
  delete [] status;
  delete res;
}

#endif /* _RETRIEVALENGINE_HPP */

