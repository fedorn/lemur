 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "OkapiRetMethod.hpp"
#include "Param.hpp"


// initial query constructor, no feedback docs assumed
OkapiQueryRep::OkapiQueryRep(TextQuery &qry, Index &dbIndex, double paramK3): ArrayQueryRep(dbIndex.termCountUnique()+1, qry, dbIndex), k3(paramK3) {
  pEst = new int[dbIndex.termCountUnique()+1];
  for (int i=0; i<=dbIndex.termCountUnique(); i++) {
    pEst[i] = 0;
  }
  pNorm = 0; 
}



double OkapiScoreFunc::matchedTermWeight(QueryTerm *qTerm, 
					 QueryRep *qRep, 
					 DocInfo *info, 
					 DocumentRep *dRep)
{
  OkapiQueryTerm * qt = (OkapiQueryTerm *)qTerm;
    //dynamic_cast<OkapiQueryTerm *>qTerm;

  OkapiQueryRep *qr = (OkapiQueryRep *)qRep;
  
  // (dynamic_cast<OkapiQueryRep *>qRep);
  return (OkapiRetMethod::RSJWeight(qt->pEstCount(), qr->pNormCount(), 
		    ind.docCount(qt->id()),
		    ind.docCount()) * 
	  dRep->termWeight(qt->id(),info) * 
	  qt->weight()); 
}


OkapiRetMethod::OkapiRetMethod(Index &dbIndex)
{
  ind = &dbIndex;
  scFunc = new OkapiScoreFunc(dbIndex);

  tfParam.k1 = OkapiParameter::defaultK1;
  tfParam.b =  OkapiParameter::defaultB;
  tfParam.k3 = OkapiParameter::defaultK3; 
  fbParam.expQTF = OkapiParameter::defaultExpQTF; 
  fbParam.howManyTerms = OkapiParameter::defaultHowManyTerms;
}




void OkapiRetMethod::updateQuery(QueryRep &origRep, DocIDSet &relDocs)
{
  
  int totalTerm=ind->termCountUnique();  
  static int * relCounts = new int[totalTerm+1];


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
    TermInfoList *tList = ind->termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      relCounts[info->id()]++;
    }
    delete tList;
  }

  IndexedRealVector weightedTerms(0);
  
  // adjust term weight for term selection
  for (i=1; i<= totalTerm; i++) {
    if (relCounts[i] >0 ) {
      weightedTerms.PushValue(i, relCounts[i]*
			      RSJWeight(relCounts[i],actualDocs, 
					ind->docCount(i),
					ind->docCount()));
    }
  }

  OkapiQueryRep *qr =  dynamic_cast<OkapiQueryRep *> (&origRep);
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
}











