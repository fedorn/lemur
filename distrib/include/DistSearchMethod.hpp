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

/** This class supports searching multiple indexes and getting a list
    of results for each of those indexes.  Each index is expected to
    have a parameter file that tells where its located and which 
    retrieval method to use, similar to parameter files used for
    TextRetrievalMethods.  If no particular RetrievalMethod is
    specified, a default one will be used (originally set to Okapi
     **/

#include "common_headers.hpp"
#include "RetrievalMethod.hpp"
#include "RetMethodManager.hpp"
#include "IndexManager.hpp"
#include "DocScore.hpp"

class DistSearchMethod {
public:
  DistSearchMethod() {csindex = NULL; defrt = RetMethodManager::OKAPI; } 
  DistSearchMethod(Index* index, RetMethodManager::RetModel defmodel=RetMethodManager::OKAPI) {csindex = index; defrt = defmodel;} 
  ~DistSearchMethod() {}

  /// use the given collection selection index 
  void setIndex(Index* index){csindex = index;} ;
  
  /// use the given retrieval method to do search if none is specified for an index
  void setDefaultRetMethod(RetMethodManager::RetModel rt) {defrt = rt;};

  /// score every document in each index in the indexset for the given query and return results in the results vector
  /*! 
    although the scores in indexset probably won't be used, take an 
    IndexedRealVector here because that's what's returned by the 
    RetrievalMethod so would make writing code easier.  the indexes in 
    indexset should correspond to indexes in the collection selection index.
  */
  void scoreIndexSet(Query &qry, IndexedRealVector &indexset, DocScoreVector** results);

  /// score every document in each index in the indexset for the given query and return results in the results vector
  void scoreIndexSet(Query &qry, vector<string> &indexset, DocScoreVector** results);

  /// convert the given IndexRealVector into a DocScoreVector.  
  static void indexToID(Index* ind, IndexedRealVector* ivec, DocScoreVector* dvec);

protected:
  /// assuming all required parameters for index and retrieval method are on the top stack, do retrieval on this single index using the given query.  
  void doSingleRetr();

  Index* csindex; // the collection selection index
  RetMethodManager::RetModel defrt; // default retrieval method to use if no param is avail
  Query* query; // the query we're currently working on
  DocScoreVector** allres; // where to store all results
  int reslen; // length of allres
};
