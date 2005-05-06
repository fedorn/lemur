/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#include "OkapiRetMethod.hpp"
#include "Param.hpp"


// initial query constructor, no feedback docs assumed
lemur::retrieval::OkapiQueryRep::OkapiQueryRep(const lemur::api::TermQuery &qry, const lemur::api::Index &dbIndex, double paramK3): ArrayQueryRep(dbIndex.termCountUnique()+1, qry, dbIndex), k3(paramK3) {
  pEst = new int[dbIndex.termCountUnique()+1];
  for (int i=0; i<=dbIndex.termCountUnique(); i++) {
    pEst[i] = 0;
  }
  pNorm = 0; 
}



double lemur::retrieval::OkapiScoreFunc::matchedTermWeight(const lemur::api::QueryTerm *qTerm, 
                                                           const lemur::api::TextQueryRep *qRep, 
                                                           const lemur::api::DocInfo *info, 
                                                           const lemur::api::DocumentRep *dRep) const
{
  const OkapiQueryTerm * qt;
  qt = dynamic_cast<const OkapiQueryTerm *> (qTerm);

  const OkapiQueryRep *qr;
  qr = dynamic_cast<const OkapiQueryRep *> (qRep);

  return (OkapiRetMethod::RSJWeight(qt->pEstCount(), qr->pNormCount(), 
                                    ind.docCount(qt->id()),
                                    ind.docCount()) * 
          dRep->termWeight(qt->id(),info) * 
          qt->weight()); 
}


lemur::retrieval::OkapiRetMethod::OkapiRetMethod(const lemur::api::Index &dbIndex, lemur::api::ScoreAccumulator &accumulator):
  lemur::api::TextQueryRetMethod(dbIndex, accumulator)
{
  scFunc = new OkapiScoreFunc(dbIndex);

  tfParam.k1 = OkapiParameter::defaultK1;
  tfParam.b =  OkapiParameter::defaultB;
  tfParam.k3 = OkapiParameter::defaultK3; 
  fbParam.expQTF = OkapiParameter::defaultExpQTF; 
  fbParam.howManyTerms = OkapiParameter::defaultHowManyTerms;
}




void lemur::retrieval::OkapiRetMethod::updateTextQuery(lemur::api::TextQueryRep &origRep, const lemur::api::DocIDSet &relDocs)
{
  
  lemur::api::COUNT_T totalTerm=ind.termCountUnique();  
  int * relCounts = new int[totalTerm+1];


  lemur::api::TERMID_T i;
  for (i=1;i<=totalTerm;i++) {
    relCounts[i]=0;
  }

  int actualDocs=0;

  relDocs.startIteration();
  while (relDocs.hasMore()) {
    int docID;
    double relPr;
    relDocs.nextIDInfo(docID, relPr);
    actualDocs++;
    lemur::api::TermInfoList *tList = ind.termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      lemur::api::TermInfo *info = tList->nextEntry();
      relCounts[info->termID()]++;
    }
    delete tList;
  }

  lemur::api::IndexedRealVector weightedTerms(0);
  weightedTerms.clear();

  // adjust term weight for term selection
  for (i=1; i<= totalTerm; i++) {
    if (relCounts[i] >0 ) {
      weightedTerms.PushValue(i, relCounts[i]*
                              RSJWeight(relCounts[i],actualDocs, 
                                        ind.docCount(i),
                                        ind.docCount()));
    }
  }

  OkapiQueryRep *qr;
  qr =  dynamic_cast<OkapiQueryRep *> (&origRep);

  qr->setPNormCount(actualDocs);
  weightedTerms.Sort();
  lemur::api::IndexedRealVector::iterator j;
  lemur::api::COUNT_T termCount=0;
  for (j= weightedTerms.begin();j!=weightedTerms.end();j++) {
    if (termCount++ >= fbParam.howManyTerms) {
      break;
    } else {
      // add the term to the query representation
      qr->incPEst((*j).ind, relCounts[(*j).ind]);
      qr->incCount((*j).ind, fbParam.expQTF);
  
      
    } 
  }
  delete [] relCounts;
}
