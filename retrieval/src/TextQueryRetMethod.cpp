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

#include "TextQueryRetMethod.hpp"
#include "BasicDocInfoList.hpp"

void TextQueryRetMethod::scoreCollection(int docID, 
					 IndexedRealVector &results){
  QueryRep *rep = computeTextQueryRep(docID);
  scoreCollection(*rep, results);
  delete(rep);
}

void TextQueryRetMethod::scoreCollection(QueryRep &qry, 
					 IndexedRealVector &results) {
  scoreInvertedIndex(qry, results);
}

void TextQueryRetMethod::scoreInvertedIndex(QueryRep &qRep, 
					    IndexedRealVector &scores, 
					    bool scoreAll) {

  scAcc.reset();
  int i;
  TextQueryRep *textQR = (TextQueryRep *)(&qRep);
  textQR->startIteration();
  
  while (textQR->hasMore()) {
    QueryTerm *qTerm = textQR->nextTerm();
    DocInfoList *dList = ind.docInfoList(qTerm->id());
    DocumentRep *dRep;
    dList->startIteration();
    while (dList->hasMore()) {
      DocInfo *info = dList->nextEntry();
      int id = info->docID();
      if (cacheDocReps) {
	if (docReps[id] == NULL)
	  docReps[id] = computeDocRep(id);      
      dRep = docReps[id];
      } else {
	dRep = computeDocRep(id);
      }
      scAcc.incScore(id, scoreFunc()->matchedTermWeight(qTerm, textQR, info, 
							dRep));
      if (! cacheDocReps)
	delete(dRep);
    }
    delete dList;
    delete qTerm;
  }

  scores.clear();
  double s;
  DocumentRep *dr;
  for (i=1; i<=ind.docCount(); i++) {
    dr = NULL;
    if (scAcc.findScore(i,s)) {
      if (cacheDocReps)
	dr = docReps[i]; // must be there if there is a score.
      else
	dr = computeDocRep(i);
      scores.PushValue(i, scoreFunc()->adjustedScore(s, textQR, dr));
      if (! cacheDocReps)
	delete(dr);
    } else if (scoreAll) {
      if (cacheDocReps) {
	if (docReps[i] == NULL)
	  docReps[i] = computeDocRep(i);
	dr = docReps[i]; // must be there.
      } else {
	dr = computeDocRep(i);
      }
      scores.PushValue(i, scoreFunc()->adjustedScore(0, textQR, dr));
      if (! cacheDocReps)
	delete(dr);
    }
  }
}

double TextQueryRetMethod::scoreDoc(QueryRep &qry, int docID) {
  HashFreqVector docVector(ind,docID);
  return (scoreDocVector(*((TextQueryRep *)(&qry)),docID,docVector));
}

double TextQueryRetMethod::scoreDocVector(TextQueryRep &qRep, int docID, 
					  FreqVector &docVector) {

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




