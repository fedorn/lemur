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


#include "InQueryRetMethod.hpp"
#include "StructQryDocRep.hpp"
#include <cmath>

InQueryRetMethod::InQueryRetMethod(Index &dbIndex, double belief,
				   int fbTerms, double fbCoef, bool cacheIDF):
  StructQueryRetMethod(dbIndex), fbCoeff(fbCoef), fbTermCount(fbTerms), 
  defaultBelief(belief) {
  docCount = dbIndex.docCount();
  docLengthAverage = dbIndex.docLengthAvg();
    // pre-compute IDF values
  if (cacheIDF) {
    int termCount = dbIndex.termCountUnique();
    double denom = log(docCount + 1.0);
    double numer = docCount + 0.5;
    idfV = new double[termCount + 1];
    idfV[0] = 0; // make sure an entry for OOV terms.
    for (int i = 1; i <= termCount; i++) {
      idfV[i] = log(numer/(double)dbIndex.docCount(i))/denom;
    }
  } else {
    idfV = NULL;
  }
  scFunc = new StructQueryScoreFunc();
}

void InQueryRetMethod::updateStructQuery(StructQueryRep &qryRep, 
					 DocIDSet &relDocs) {
  // build a top query node of wsum, choose new terms as is done by
  // TFIDF
  int totalTerm = ind.termCountUnique();  
  float * expandTerms = new float[totalTerm + 1]; // one extra for OOV
  int i;
  for (i = 1; i <= totalTerm; i++) {
    expandTerms[i] = 0;
  }
  relDocs.startIteration();
  int docCount = ind.docCount();
  double denom = log(docCount + 1.0);
  double numer = docCount + 0.5;
  while (relDocs.hasMore()) {
    double relPr;
    int docID;
    relDocs.nextIDInfo(docID, relPr);
    StructQryDocRep *dr = (StructQryDocRep *)computeDocRep(docID);
    TermInfoList *tList = ind.termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      int tid = info->id();
      double dtf = (double)info->count();
      if (idfV != NULL)
	expandTerms[tid] += fbCoeff * (dr->beliefScore(dtf, idfV[tid]));
      else      
	expandTerms[tid] += fbCoeff * (dr->beliefScore(dtf, log(numer/(double)ind.docCount(tid))/denom));
    }
    delete(dr);
    delete(tList);
  }
  IndexedRealVector expTerms;
  for (i = 1; i <= totalTerm; i++) {
    if (expandTerms[i] > 0) {
      IndexedReal entry;
      entry.ind = i;
      entry.val = expandTerms[i];
      expTerms.push_back(entry);
    }
  }
  expTerms.Sort();
  QueryNode *expandQry = new WsumQnode(1.0);
  QnList *chlist = new QnList();
  //  expandQry->ch = chlist;
  expandQry->setChildren(chlist);
  QueryNode *qn = qryRep.topnode();
  // weight the original query
  qn->setWeight(1 - fbCoeff);
  // add the original query as the first child of the new wsum
  chlist->push_back(qn);
  
  IndexedRealVector::iterator j;
  int termCount = 0;
  for (j = expTerms.begin(); 
       termCount < fbTermCount && j != expTerms.end(); 
       j++, termCount++) {
    // adding the expanded terms to the query
    int tid = (*j).ind;
    double belief = (*j).val;
    QueryNode *expQTerm = new TermQnode(tid, belief);
    DocInfoList *dl = ind.docInfoList(tid);
    int listlen = ind.docCount(tid);
    int tf = ind.termCount(tid);
    expQTerm->copyDocList(listlen, tf, dl, docCount);
    chlist->push_back(expQTerm);
  }
  delete[](expandTerms);
  expandQry->updateDocList(docCount);
  expandQry->setEntries(termCount + 1);
  qryRep.setTopnode(expandQry);
}
