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


#include "InQueryRetMethod.hpp"
#include "StructQryDocRep.hpp"
#include <cmath>

lemur::retrieval::InQueryRetMethod::InQueryRetMethod(const lemur::api::Index &dbIndex, double belief,
                                                     int fbTerms, double fbCoef, bool cacheIDF):
  StructQueryRetMethod(dbIndex), fbCoeff(fbCoef), fbTermCount(fbTerms), 
  defaultBelief(belief) {
  docCount = dbIndex.docCount();
  docLengthAverage = dbIndex.docLengthAvg();
  // pre-compute IDF values
  if (cacheIDF) {
    lemur::api::COUNT_T termCount = dbIndex.termCountUnique();
    double denom = log(docCount + 1.0);
    double numer = docCount + 0.5;
    idfV = new double[termCount + 1];
    idfV[0] = 0; // make sure an entry for OOV terms.
    for (lemur::api::TERMID_T i = 1; i <= termCount; i++) {
      idfV[i] = log(numer/(double)dbIndex.docCount(i))/denom;
    }
  } else {
    idfV = NULL;
  }
  scFunc = new lemur::api::StructQueryScoreFunc();
}

void lemur::retrieval::InQueryRetMethod::updateStructQuery(StructQueryRep &qryRep, 
                                                           const lemur::api::DocIDSet &relDocs) {
  // build a top query node of wsum, choose new terms as is done by
  // TFIDF
  lemur::api::COUNT_T totalTerm = ind.termCountUnique();  
  float * expandTerms = new float[totalTerm + 1]; // one extra for OOV
  lemur::api::TERMID_T i;
  for (i = 1; i <= totalTerm; i++) {
    expandTerms[i] = 0;
  }
  relDocs.startIteration();
  lemur::api::COUNT_T docCount = ind.docCount();
  double denom = log(docCount + 1.0);
  double numer = docCount + 0.5;
  while (relDocs.hasMore()) {
    double relPr;
    int docID;
    relDocs.nextIDInfo(docID, relPr);
    StructQryDocRep *dr = (StructQryDocRep *)computeDocRep(docID);
    lemur::api::TermInfoList *tList = ind.termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      lemur::api::TermInfo *info = tList->nextEntry();
      lemur::api::TERMID_T tid = info->termID();
      double dtf = (double)info->count();
      if (idfV != NULL)
        expandTerms[tid] += fbCoeff * (dr->beliefScore(dtf, idfV[tid]));
      else      
        expandTerms[tid] += fbCoeff * (dr->beliefScore(dtf, log(numer/(double)ind.docCount(tid))/denom));
    }
    delete(dr);
    delete(tList);
  }
  lemur::api::IndexedRealVector expTerms;
  for (i = 1; i <= totalTerm; i++) {
    if (expandTerms[i] > 0) {
      lemur::api::IndexedReal entry;
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
  
  lemur::api::IndexedRealVector::iterator j;
  lemur::api::COUNT_T termCount = 0;
  for (j = expTerms.begin(); 
       termCount < fbTermCount && j != expTerms.end(); 
       j++, termCount++) {
    // adding the expanded terms to the query
    lemur::api::TERMID_T tid = (*j).ind;
    double belief = (*j).val;
    QueryNode *expQTerm = new TermQnode(tid, belief);
    lemur::api::DocInfoList *dl = ind.docInfoList(tid);
    lemur::api::COUNT_T listlen = ind.docCount(tid);
    lemur::api::COUNT_T tf = ind.termCount(tid);
    expQTerm->copyDocList(listlen, tf, dl, docCount);
    chlist->push_back(expQTerm);
  }
  delete[](expandTerms);
  expandQry->updateDocList(docCount);
  expandQry->setEntries(termCount + 1);
  qryRep.setTopnode(expandQry);
}
