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



#include "TextQueryRetMethod.hpp"


#include "BasicDocInfoList.hpp"

void TextQueryRetMethod::scoreCollection(QueryRep &qry, IndexedRealVector &results)
{
  scoreInvertedIndex(qry, results);
}






void TextQueryRetMethod::scoreInvertedIndex(QueryRep &qRep, IndexedRealVector &scores, bool scoreAll)
{

  scAcc.reset();

  int numDocs = ind.docCount();
  int i;
  TextQueryRep *textQR = (TextQueryRep *)(&qRep);
  textQR->startIteration();
  
  while (textQR->hasMore()) {
    QueryTerm *qTerm = textQR->nextTerm();
    
    DocInfoList *dList = ind.docInfoList(qTerm->id());
    dList->startIteration();
    while (dList->hasMore()) {
	
      DocInfo *info = dList->nextEntry();
      DocumentRep *dRep = computeDocRep(info->docID());
      scAcc.incScore(info->docID(), scoreFunc()
		     ->matchedTermWeight(qTerm, textQR, info, dRep));
      delete dRep;
    }
    delete dList;
    delete qTerm;
  }

  scores.clear();
  double s;
  DocumentRep *dr;
  for (i=1; i<=ind.docCount(); i++) {
    if (scAcc.findScore(i,s)) {
      dr = computeDocRep(i);
      scores.PushValue(i, scoreFunc()->adjustedScore(s, textQR, dr));
    } else if (scoreAll) {
      dr = computeDocRep(i);
      scores.PushValue(i, scoreFunc()->adjustedScore(0, textQR, dr));
    }
  }
}


double TextQueryRetMethod::scoreDoc(QueryRep &qry, int docID)
{
  HashFreqVector docVector(ind,docID);
  return (scoreDocVector(*((TextQueryRep *)(&qry)),docID,docVector));
}

double TextQueryRetMethod::scoreDocVector(TextQueryRep &qRep, int docID, FreqVector &docVector)
{

  qRep.startIteration();
  
  double score=0;

  DocumentRep *dRep = computeDocRep(docID);

  BasicDocInfo *dInfo;
  while (qRep.hasMore()) {
    QueryTerm *qTerm = qRep.nextTerm();
    int fq;
    
    if (docVector.find(qTerm->id(),fq)) {
      dInfo = new BasicDocInfo(docID, fq);
      score += scoreFunc()
	->matchedTermWeight(qTerm, &qRep, dInfo, dRep);
      delete dInfo;
    }
    delete qTerm;
  }
  score = scoreFunc()->adjustedScore(score, &qRep,  dRep);
  delete dRep;
  return score;
}




