#include "RetrievalEngine.hpp"


RetrievalEngine::RetrievalEngine(RetrievalMethod &retMethod): method(retMethod) , ind(retMethod.index()){
  
  scAcc = new double[ind->docCount()+1]; 
  // one more entry, because zero index is invalid 

  status = new int[ind->docCount()+1]; 
  // status is needed to indicate which entry is a valid score
  // note that zero could be a valid score!
  
  res = new IndexedRealVector();

}

void RetrievalEngine::retrieve(TextQuery &qry, IndexedRealVector *&results)
{
  qryRep = method.computeQueryRep(qry);
  scoreInvertedIndex(*qryRep, *res);
  results = res;
  delete qryRep;

}

void RetrievalEngine::retrievePseudoFeedback(TextQuery &qry, int howManyDoc, IndexedRealVector *&results)
{
  qryRep = method.computeQueryRep(qry);
  scoreInvertedIndex(*qryRep, *res);
  PseudoFBDocs *topDoc = new PseudoFBDocs(*res, howManyDoc);
  method.updateQuery(*qryRep,*topDoc);
  scoreInvertedIndex(*qryRep, *res);
  results = res;
  delete qryRep;
}


void RetrievalEngine::scoreInvertedIndex(QueryRep &qRep, IndexedRealVector &sortedScores)
{

  int numDocs = ind->docCount();

  int i;
  for (i=1; i<=numDocs; i++) {
    scAcc[i]= 0;
    status[i]=0;
  }

  qRep.startIteration();
  
  while (qRep.hasMore()) {
    QueryTerm *qTerm = qRep.nextTerm();
    
    DocInfoList *dList = ind->docInfoList(qTerm->id());
    dList->startIteration();
    while (dList->hasMore()) {
	
      DocInfo *info = dList->nextEntry();
      DocumentRep *dRep = method.computeDocRep(info->docID());
      scAcc[info->docID()] += method.scoreFunc()
	->matchedTermWeight(qTerm, &qRep, info, dRep);
      status[info->docID()]=1;
      delete dRep;
    }
    delete dList;
    delete qTerm;
  }

  sortedScores.clear();
  for (i=1; i<= numDocs; i++) {
    if (status[i]) { // a valid entry
      // copy scores 
      IndexedReal entry;
      entry.ind = i;
      DocumentRep *dr = method.computeDocRep(i);
      entry.val = method.scoreFunc()->adjustedScore(scAcc[i], &qRep, dr);
      sortedScores.push_back(entry);
      delete dr;
    }
  }
  sortedScores.Sort();
}












