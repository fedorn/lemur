
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
 
#include "CORIRetMethod.hpp"

#include <stdio.h>

CORIDocRep::CORIDocRep(int docID, Index & dbIndex, double * cwRatio, 
		       double TFfact, double TFbase, 
		       SimpleKLDocModel * smoother,
		       UnigramLM * collectLM) : 
  DocumentRep(docID), ind(dbIndex) {
   

  double c = ind.docCount();
  idiv = log (c + 1);    
  c05 = c + 0.5;
  
  dfSmooth = smoother;
  collLM = collectLM;

  tnorm = TFbase + TFfact * cwRatio[docID-1];
}

double 
CORIDocRep::termWeight(int termID, DocInfo * info) { 
  
  double cf = ind.docCount(termID);
  double df = info->termCount(); 

  if (dfSmooth != NULL) {
    cout<<"smooth:"<<endl;
    if (df > 0) {
      df = dfSmooth->seenProb(df, termID) * ind.docLength(info->docID());
    } else {
      df = dfSmooth->unseenCoeff() * collLM->prob(termID) * 
	ind.docLength(info->docID());
    }
  }
  
  double t = df / (df + tnorm);
  double i = log (c05/ cf) / idiv;
  double p = (1-MINBELIEF) * t * i;
  
  //  cout << ind.term(termID) << " " << ind.document(info->docID())
  //       << " " << df << " " << cf << endl;
  if (t>1){
       cout << ind.term(termID) << " " << ind.document(info->docID())
         << " " << df << " " << cf << endl;
  }
  return p;
}

CORIQueryRep::CORIQueryRep(TextQuery &qry, Index &dbIndex) : 
  ArrayQueryRep (dbIndex.termCountUnique() + 1, qry, dbIndex), ind(dbIndex) {

  startIteration();
  while (hasMore()) {
    QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight());
  }
}

void CORIRetMethod::scoreCollection(QueryRep &qry, 
				    IndexedRealVector &results) {
  scoreInvertedIndex(qry, results, false);
  //adjust the score;

  double c = ind.docCount();
  double c05 = c + 0.5;
  double idiv = log (c + 1);    
  double rmax=0;
  double rmin=0;

  TextQueryRep *textQry = (TextQueryRep *)(&qry);
  textQry->startIteration();
  rmax = 0;
  double qw = 0;
  while (textQry->hasMore()) {
    int qtid = textQry->nextTerm()->id();
    rmax += (1-MINBELIEF)*(log(c05 / ind.docCount(qtid)) / idiv);
  }

  for (int i=0;i<results.size();i++) {
    int dbOverlapNum=0; //the number of overlap documents for this specific database
    results[i].val/=rmax;
  }
}



CORIRetMethod::CORIRetMethod(Index & dbIndex, ScoreAccumulator &accumulator, 
				   String cwName, int isCSIndex=0,
			     SimpleKLDocModel ** smoothers,
			     UnigramLM * collectLM) : 
  TextQueryRetMethod (dbIndex, accumulator) {


  scFunc = new CORIScoreFunc(dbIndex);
  dfSmooth = smoothers;
  collLM = collectLM;

  //set defaults according the whether this index is collection selection index
  //or normal doc retrieval index
  if (isCSIndex==1){
    tffactor = CSTFFACTOR;
    tfbaseline = CSTFBASELINE;
  }else{
    tffactor=DOCTFFACTOR;
    tfbaseline=DOCTFBASELINE;
  }

  //cout<<tffactor<<"  "<<tfbaseline<<endl;
  int dc = ind.docCount();

  cwRatio = new double[dc];

  if (cwName.compare("USE_INDEX_COUNTS") == 0) {
    float ave = ind.docLengthAvg();
    for (int i=0;i<dc;i++) {
      cwRatio[i] = ind.docLength(i+1)/ave;
    }
  } else {
    int * cwCount = new int[dc];

    FILE * fp = fopen(cwName.c_str(), "rb");
    int * cw = cwCount; int d = 0;
    while (d < dc) {
      int dd = fread(cw, sizeof(int), dc - d, fp);
      d += dd;
      cw += dd;
    }
  
    fclose(fp);
  

    double cwTot=0;
    int i;
    for (i = 0; i < dc; i++) {
      cwTot += cwCount[i];
    }
    double cwAve = cwTot / dc;
    for (i = 0; i < dc; i++) {
      cwRatio[i] = cwCount[i] / cwAve;
    }

    delete [] cwCount;
  }
}

