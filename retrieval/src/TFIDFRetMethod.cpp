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


#include "TFIDFRetMethod.hpp"
#include "Param.hpp"
#include <cmath>


TFIDFQueryRep::TFIDFQueryRep(TextQuery &qry, Index &dbIndex, double *idfValue, TFIDFParameter::WeightParam &param): ArrayQueryRep (dbIndex.termCountUnique()+1, qry, dbIndex), ind(dbIndex), idf(idfValue), prm(param)
{
  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    setCount(qt->id(), queryTFWeight(qt->weight())*idf[qt->id()]);
    // cout << "term : "<< dbIndex.term(qt->id()) << " idf="<< idf[qt->id()] <<    " total "<< dbIndex.docCount() << " with term "<< dbIndex.docCount(qt->id()) << endl;
    delete qt;
  }
}
double TFIDFQueryRep::queryTFWeight(const double rawTF)
{
  if (prm.tf == TFIDFParameter::RAWTF) {
    return (rawTF);
  } else if (prm.tf == TFIDFParameter::LOGTF) {
    return (log(rawTF+1));
  } else if (prm.tf == TFIDFParameter::BM25) {
    return (TFIDFRetMethod::BM25TF(rawTF,prm.bm25K1,0,
		   1, 1));  // no length normalization for query 
  } else {  // default to raw TF
    cerr << "Warning: unknown TF method, raw TF assumed\n";
    return rawTF;
  }
}



double TFIDFDocRep::docTFWeight(const double rawTF)
{
  if (prm.tf == TFIDFParameter::RAWTF) {
    return (rawTF);
  } else if (prm.tf == TFIDFParameter::LOGTF) {
    return (log(rawTF+1));
  } else if (prm.tf == TFIDFParameter::BM25) {
    
    return (TFIDFRetMethod::BM25TF(rawTF, prm.bm25K1, prm.bm25B,
		   ind.docLength(id), ind.docLengthAvg()));
  } else {  // default to raw TF
    cerr << "Warning: unknown TF method, raw TF assumed\n";
    return rawTF;
  }
}


TFIDFRetMethod::TFIDFRetMethod(Index &dbIndex, ScoreAccumulator &accumulator) :TextQueryRetMethod(dbIndex, accumulator) 
{
  // set default parameter value
  docTFParam.tf = TFIDFParameter::BM25;
  docTFParam.bm25K1 = TFIDFParameter::defaultDocK1;
  docTFParam.bm25B = TFIDFParameter::defaultDocB;
  
  qryTFParam.tf = TFIDFParameter::BM25;
  qryTFParam.bm25K1 = TFIDFParameter::defaultQryK1;
  qryTFParam.bm25B = TFIDFParameter::defaultQryB;
 
  fbParam.howManyTerms = TFIDFParameter::defaultHowManyTerms;
  fbParam.posCoeff = TFIDFParameter::defaultPosCoeff;

  // pre-compute IDF values
  idfV = new double[dbIndex.termCountUnique()+1];
  for (int i=1; i<=dbIndex.termCountUnique(); i++) {
    idfV[i] = log((dbIndex.docCount()+1)/(0.5+dbIndex.docCount(i)));
  }
  scFunc = new ScoreFunction();
}




void TFIDFRetMethod::updateTextQuery(TextQueryRep &qryRep, DocIDSet &relDocs)
{
  int totalTerm=ind.termCountUnique();  
  static float * centroidVector = new float[totalTerm+1]; // one extra for OOV

  int i;
  for (i=1;i<=totalTerm;i++) {
    centroidVector[i]=0;
  }

  int actualDocs=0;
  relDocs.startIteration();
  while (relDocs.hasMore()) {
    int docID;
    double relPr;
    relDocs.nextIDInfo(docID, relPr);
    actualDocs++;

    TermInfoList *tList = ind.termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      TFIDFDocRep *dr;
      dr = dynamic_cast<TFIDFDocRep *>(computeDocRep(docID));
      centroidVector[info->id()] += dr->docTFWeight(info->count());
      delete dr;
    }
    delete tList;
  }

  // adjust term weight to obtain true Rocchio weight
  for (i=1; i<= totalTerm; i++) {
    if (centroidVector[i] >0 ) {
      centroidVector[i] *= idfV[i]/actualDocs;
    }
  }

  IndexedRealVector centVector(0);
  
  for (i=1; i< totalTerm; i++) {
    if (centroidVector[i]>0) {
      IndexedReal entry;
      entry.ind = i;
      entry.val = centroidVector[i];
      centVector.push_back(entry);
    }
  }
  centVector.Sort();
  IndexedRealVector::iterator j;
  int termCount=0;
  for (j= centVector.begin();j!=centVector.end();j++) {
    if (termCount++ >= fbParam.howManyTerms) {
      break;
    } else {
      // add the term to the query vector
      (dynamic_cast<TFIDFQueryRep *>(&qryRep))->incCount((*j).ind, (*j).val*fbParam.posCoeff);
    }
  }

}



