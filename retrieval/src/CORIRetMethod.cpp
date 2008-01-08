
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
 
#include "CORIRetMethod.hpp"

#include <stdio.h>

lemur::retrieval::CORIDocRep::CORIDocRep(lemur::api::DOCID_T docID, 
                                         const lemur::api::Index & dbIndex, 
                                         double * cwRatio, 
                                         double TFfact, double TFbase, 
                                         const SimpleKLDocModel * smoother,
                                         const lemur::langmod::UnigramLM * collectLM) : 
  lemur::api::DocumentRep(docID, dbIndex.docLength(docID)), ind(dbIndex) {
   
  double c = ind.docCount();
  idiv = log (c + 1);    
  c05 = c + 0.5;
  
  dfSmooth = smoother;
  collLM = collectLM;

  tnorm = TFbase + TFfact * cwRatio[docID-1];
}

double 
lemur::retrieval::CORIDocRep::termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo * info) const { 
  
  double cf = ind.docCount(termID);
  double df = info->termCount(); 

  if (dfSmooth != NULL) {
    cout<<"smooth:"<<endl;
    if (df > 0) {
      df = dfSmooth->seenProb(df, termID) * docLength;
    } else {
      df = dfSmooth->unseenCoeff() * collLM->prob(termID) * docLength;
    }
  }
  
  double t = df / (df + tnorm);
  double i = log (c05/ cf) / idiv;
  double p = (1-MINBELIEF) * t * i;
  
  return p;
}

lemur::retrieval::CORIQueryRep::CORIQueryRep(const lemur::api::TermQuery &qry, const lemur::api::Index &dbIndex) : 
  ArrayQueryRep (dbIndex.termCountUnique() + 1, qry, dbIndex), ind(dbIndex) {

  startIteration();
  while (hasMore()) {
    lemur::api::QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight());
  }
}

void lemur::retrieval::CORIRetMethod::scoreCollection(const lemur::api::QueryRep &qry, 
                                                      lemur::api::IndexedRealVector &results) {
  scoreInvertedIndex(qry, results, false);
  //adjust the score;

  double c = ind.docCount();
  double c05 = c + 0.5;
  double idiv = log (c + 1);    
  double rmax=0;
  double rmin=0;

  lemur::api::TextQueryRep *textQry = (lemur::api::TextQueryRep *)(&qry);
  textQry->startIteration();
  rmax = 0;
  double qw = 0;
  while (textQry->hasMore()) {
    lemur::api::QueryTerm *qt = textQry->nextTerm();
    lemur::api::TERMID_T qtid = qt->id();
    // have to adjust for query term weight
    rmax += (1-MINBELIEF)*(log(c05 / ind.docCount(qtid)) / idiv)*qt->weight();
  }

  for (int i=0;i<results.size();i++) {
    int dbOverlapNum=0; //the number of overlap documents for this specific database
    results[i].val/=rmax;
  }
}



lemur::retrieval::CORIRetMethod::CORIRetMethod(const lemur::api::Index & dbIndex, lemur::api::ScoreAccumulator &accumulator, 
                                               lemur::utility::String cwName, int isCSIndex,
                                               const SimpleKLDocModel ** smoothers,
                                               const lemur::langmod::UnigramLM * collectLM) : 
  lemur::api::TextQueryRetMethod (dbIndex, accumulator) {


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

  lemur::api::COUNT_T dc = ind.docCount();

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

