/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

/** czhai */

#include "RetrievalMethod.hpp"


// the most general, but probably also most inefficient implementation
void lemur::api::RetrievalMethod::scoreCollection(const QueryRep &qry, IndexedRealVector &results)
{
  results.clear();
  for (DOCID_T id=1; id<=ind.docCount();id++) {
    results.PushValue(id, scoreDoc(qry, id));
  }
}

/// Score a set of documents w.r.t. a query rep (e.g. for re-ranking)
void lemur::api::RetrievalMethod::scoreDocSet(const QueryRep &qry, const DocIDSet &docSet, IndexedRealVector &results)
{
  results.clear();
  docSet.startIteration();
  while (docSet.hasMore()) {
    int docID;
    double prevScore;
    docSet.nextIDInfo(docID, prevScore);
    results.PushValue(docID, scoreDoc(qry, docID));
  }
}

