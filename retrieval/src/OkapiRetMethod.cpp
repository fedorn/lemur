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


#include "OkapiRetMethod.hpp"
#include "Param.hpp"


// initial query constructor, no feedback docs assumed
OkapiQueryRep::OkapiQueryRep(const TextQuery &qry, const Index &dbIndex, double paramK3): ArrayQueryRep(dbIndex.termCountUnique()+1, qry, dbIndex), k3(paramK3) {
  pEst = new int[dbIndex.termCountUnique()+1];
  for (int i=0; i<=dbIndex.termCountUnique(); i++) {
    pEst[i] = 0;
  }
  pNorm = 0; 
}



double OkapiScoreFunc::matchedTermWeight(const QueryTerm *qTerm, 
					 const TextQueryRep *qRep, 
					 const DocInfo *info, 
					 const DocumentRep *dRep) const
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


OkapiRetMethod::OkapiRetMethod(const Index &dbIndex, ScoreAccumulator &accumulator):
  TextQueryRetMethod(dbIndex, accumulator)
{
  scFunc = new OkapiScoreFunc(dbIndex);

  tfParam.k1 = OkapiParameter::defaultK1;
  tfParam.b =  OkapiParameter::defaultB;
  tfParam.k3 = OkapiParameter::defaultK3; 
  fbParam.expQTF = OkapiParameter::defaultExpQTF; 
  fbParam.howManyTerms = OkapiParameter::defaultHowManyTerms;
}




void OkapiRetMethod::updateTextQuery(TextQueryRep &origRep, const DocIDSet &relDocs)
{
  
  int totalTerm=ind.termCountUnique();  
  int * relCounts = new int[totalTerm+1];


  int i;
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
    TermInfoList *tList = ind.termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      relCounts[info->termID()]++;
    }
    delete tList;
  }

  IndexedRealVector weightedTerms(0);
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
  IndexedRealVector::iterator j;
  int termCount=0;
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











