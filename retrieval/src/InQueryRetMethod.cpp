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
#include "Param.hpp"
#include <cmath>

double StructQryDocRep::termWeight(int termID, DocInfo *info) {
  // docinfo is useless here, keep it for an identical termWeight api
  double dtf=(double)docTermFrq->count(termID);
  return beliefScore(dtf, idf[termID]);
}

double StructQryDocRep::computeIdfScore(double df) {
  return log((double)(ind.docCount()+0.5)/(double)df)/(log((double)ind.docCount())+1);
}

double StructQryDocRep::beliefScore(double df, double idf) {
  double a = InQueryParameter::defaultBelief;
  return (a+(1-a)*(df/(df+0.5+1.5*(passageLength()/ind.docLengthAvg())))*idf);
}

InQueryRetMethod::InQueryRetMethod(InvFPIndex &dbIndex, ScoreAccumulator &accumulator) : 
  StructQueryRetMethod(dbIndex, accumulator) {

  docPrm.defaultBelief = InQueryParameter::defaultBelief;
  qryPrm.fbCoeff = InQueryParameter::defaultFBCoeff;
  qryPrm.fbTermCount = InQueryParameter::defaultFBTermCount;

    // pre-compute IDF values
  idfV = new double[dbIndex.termCountUnique()+1];
  for (int i=1; i<=dbIndex.termCountUnique(); i++) {
    idfV[i] = log((double)(dbIndex.docCount()+0.5)/(double)dbIndex.docCount(i))/(log((double)(dbIndex.docCount())+1));
  }

  scFunc = new StructQueryScoreFunc();
}

void InQueryRetMethod::updateStructQuery(StructQueryRep &qryRep, DocIDSet &relDocs)
{
  // build a top query node of wsum, choose new terms as is done by
  // TFIDF
  int totalTerm=index.termCountUnique();  
  static float * expandTerms = new float[totalTerm+1]; // one extra for OOV

  int i;
  for (i=1;i<=totalTerm;i++) {
    expandTerms[i]=0;
  }

  relDocs.startIteration();
  while (relDocs.hasMore()) {
    double relPr;
    int docID;
    relDocs.nextIDInfo(docID, relPr);
    StructQryDocRep *dr= new StructQryDocRep((int)docID, (InvFPIndex &)ind, &idfV[0]);
    for (i=1;i<=totalTerm;i++) {
      double dtf=(double)(dr->docTermFrq->count(i));
      if(dtf>0)  // rel part of Rocchio weight, i.e. beta*X_rel 
	expandTerms[i] +=qryPrm.fbCoeff * (dr->beliefScore(dtf, idfV[i]));
    }
    delete dr;
  }

  IndexedRealVector expTerms(0);
 
  for (i=1; i<= totalTerm; i++) {
    if (expandTerms[i]>0) {
      IndexedReal entry;
      entry.ind = i;
      entry.val = expandTerms[i];
      expTerms.push_back(entry);
    }
  }
  expTerms.Sort();
  QueryNode *expandQry = new WsumQnode(1.0);
  QnList *chlist= new QnList();
  expandQry->ch=chlist;
  
  QueryNode *qn=qryRep.topnode();
  // weight the original query
  qn->setWeight(1-qryPrm.fbCoeff);
  // add the original query as the first child of the new wsum
  chlist->push_back(qn);
  
  IndexedRealVector::iterator j;
  int termCount=0;
  for (j= expTerms.begin();j!=expTerms.end();j++) {
    if (termCount++ >= qryPrm.fbTermCount) {
      break;
    } else {
      // adding the expanded terms to the query
      QueryNode *expQTerm = new TermQnode((*j).ind, (*j).val);
      DocInfoList *dl = ind.docInfoList((*j).ind);
      qryRep.copyDocList(dl, expQTerm);
      delete dl;
      chlist->push_back(expQTerm);
    }
  }
  qryRep.unionDocList(expandQry);
  expandQry->entries=termCount+1;
  qryRep.setTopnode(expandQry);
}

