/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  Author: fff
 */

#include "StructQueryRetMethod.hpp"
#include "StructQryDocRep.hpp"
#include "BasicDocInfoList.hpp"

void StructQueryRetMethod::scoreCollection(QueryRep &qry, 
					   IndexedRealVector &results) {
  scoreInvertedIndex(qry, results);
}


void StructQueryRetMethod::scoreInvertedIndex(QueryRep &qRep, 
					      IndexedRealVector &scores, 
					      bool scoreAll) {
  int numDocs = ind.docCount();
  int i;
  QueryNode *queryRoot = ((StructQueryRep *)(&qRep))->topnode();
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

double StructQueryRetMethod::scoreDoc(QueryRep &qry, int docID) {
  double score = 0;
  QueryNode *structQR = ((StructQueryRep &)qry).topnode();
  DocumentRep *dRep = computeDocRep(docID);
  score = ((StructQueryScoreFunc *)scoreFunc())->evalQuery(structQR, dRep);
  delete dRep;
  return score;
}


