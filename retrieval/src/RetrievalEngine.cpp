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


#include "RetrievalEngine.hpp"
#include "BasicDocInfoList.hpp"

RetrievalEngine::RetrievalEngine(RetrievalMethod &retMethod): method(retMethod) , ind(retMethod.index()){
  
  scAcc = new double[ind->docCount()+1]; 
  // one more entry, because zero index is invalid 

  status = new int[ind->docCount()+1]; 
  // status is needed to indicate which entry is a valid score
  // note that zero could be a valid score!
  
  res = new IndexedRealVector();

}

void RetrievalEngine::retrieve(TextQuery &qry, IndexedRealVector *&results, bool rankAll)
{
  qryRep = method.computeQueryRep(qry);
  if (rankAll) {
    scoreAll(*qryRep, *res);
  } else {
    scoreInvertedIndex(*qryRep, *res);
  }
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
  delete topDoc;
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

void RetrievalEngine::scoreAll(QueryRep &qRep, IndexedRealVector &sortedScores)
{

  int numDocs = ind->docCount();

  bool * scored  = new bool[numDocs+1];

  int i;
  for (i=1; i<=numDocs; i++) {
    scAcc[i]= 0;

  }

  qRep.startIteration();
  
  while (qRep.hasMore()) {
    QueryTerm *qTerm = qRep.nextTerm();

    for (i=1; i<=numDocs; i++) {
      scored[i] = false;
    }

    DocInfoList *dList = ind->docInfoList(qTerm->id());
    dList->startIteration();
    while (dList->hasMore()) {
      DocInfo *info = dList->nextEntry();
      int did = info->docID();
      DocumentRep *dRep = method.computeDocRep(did);
      scAcc[did] += method.scoreFunc()
	->matchedTermWeight(qTerm, &qRep, info, dRep);

      scored[did]=true;
      delete dRep;
      delete info;
    }
    delete dList;
    for (i = 1; i <= numDocs; i++) {
      if (scored[i] == false) {
	DocInfo * info = new BasicDocInfo(i, 0);
	DocumentRep * dRep = method.computeDocRep(i);
	scAcc[i] += method.scoreFunc()->matchedTermWeight(qTerm, &qRep, info, dRep);
	delete info;
	delete dRep;
      }
    }
    delete qTerm;
  }

  sortedScores.clear();
  for (i=1; i<= numDocs; i++) {
    // copy scores 
    IndexedReal entry;
    entry.ind = i;
    DocumentRep *dr = method.computeDocRep(i);
    entry.val = method.scoreFunc()->adjustedScore(scAcc[i], &qRep, dr);
    sortedScores.push_back(entry);
    delete dr;
  }
  sortedScores.Sort();

  delete [] scored;
}












