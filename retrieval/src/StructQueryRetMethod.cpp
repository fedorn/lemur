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


void StructQryDocRep::startPassageIteration() {
  start=0;
  end = size < docEnd ? size : docEnd;
  offset=0;
  maxScore=0;
}

bool StructQryDocRep::hasMorePassage(){
  // still some terms in the list.
  return(start < docEnd);
}

void StructQryDocRep::nextPassage() {
  if(start + increment < docEnd)
    start +=increment;
  else
    start = docEnd;
  end = (start+size) < docEnd ? (start+size) : docEnd;
}


void StructQueryRetMethod::scoreCollection(QueryRep &qry, IndexedRealVector &results)
{
  scoreInvertedIndex(qry, results);
}


void StructQueryRetMethod::scoreInvertedIndex(QueryRep &qRep, IndexedRealVector &scores, bool scoreAll)
{

  scAcc.reset();
  int numDocs = ind.docCount();
  int i;
  QueryNode *queryRoot = ((StructQueryRep *)(&qRep))->topnode();

  // eval the query
  scAcc.reset();
  for(i=1; i<= numDocs; i++) {
    if(queryRoot->dList[i]) {
      DocumentRep *dRep = computeDocRep(i);
      scAcc.setScore(i, ((StructQueryScoreFunc *)scoreFunc())->evalQuery(queryRoot, dRep));
      delete dRep;
    }
  }

  scores.clear();
  double s;
  for (i=1; i<=numDocs; i++) {
    if (scAcc.findScore(i,s)) {
      scores.PushValue(i, s);
    } else if (scoreAll) {
      scores.PushValue(i, ((StructQueryScoreFunc *)scoreFunc())->adjustedScore(0, queryRoot));
    }
  }
}


double StructQueryRetMethod::scoreDoc(QueryRep &qry, int docID)
{
  HashFreqVector docVector(ind,docID);
  return (scoreDocVector(*((StructQueryRep *)(&qry)),docID,docVector));
}

double StructQueryRetMethod::scoreDocVector(StructQueryRep &qRep, int docID, FreqVector &docVector)
{

  
  double score=0;
  QueryNode *structQR = (&qRep)->topnode();
  DocumentRep *dRep = computeDocRep(docID);

  score = ((StructQueryScoreFunc *)scoreFunc())->evalQuery(structQR, dRep);
  delete dRep;
  return score;
}


