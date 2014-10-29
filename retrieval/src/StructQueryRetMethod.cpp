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

#include "StructQueryRetMethod.hpp"
#include "StructQryDocRep.hpp"

void lemur::api::StructQueryRetMethod::scoreCollection(const QueryRep &qry, 
                                                       IndexedRealVector &results) {
  scoreInvertedIndex(qry, results);
}


void lemur::api::StructQueryRetMethod::scoreInvertedIndex(const QueryRep &qRep, 
                                                          IndexedRealVector &scores, 
                                                          bool scoreAll) {
  COUNT_T numDocs = ind.docCount();
  COUNT_T i;
  lemur::retrieval::QueryNode *queryRoot = ((lemur::retrieval::StructQueryRep *)(&qRep))->topnode();
  scores.clear();  
  // eval the query
  StructQueryScoreFunc * scorer = (StructQueryScoreFunc *)scoreFunc();
  for (i = 1; i <= numDocs; i++) {
    if (queryRoot->dList[i]) {
      DocumentRep *dRep = computeDocRep(i);
      scores.PushValue(i, scorer->evalQuery(queryRoot, dRep));
      delete dRep;
    } else if (scoreAll) {
      scores.PushValue(i, scorer->adjustedScore(0, queryRoot));
    }
  }
}

double lemur::api::StructQueryRetMethod::scoreDoc(const QueryRep &qry, DOCID_T docID) {
  double score = 0;
  lemur::retrieval::QueryNode *structQR = ((lemur::retrieval::StructQueryRep &)qry).topnode();
  DocumentRep *dRep = computeDocRep(docID);
  score = ((StructQueryScoreFunc *)scoreFunc())->evalQuery(structQR, dRep);
  delete dRep;
  return score;
}


