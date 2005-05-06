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
  Author: dmf
*/

#include "CosSimRetMethod.hpp"
#include "Param.hpp"
#include <cmath>

/// Construct a query rep from a text query.
lemur::retrieval::CosSimQueryRep::CosSimQueryRep(const lemur::api::TermQuery &qry, const lemur::api::Index &dbIndex, 
                                                 double *idfValue): 
  ArrayQueryRep (dbIndex.termCountUnique()+1, qry, dbIndex), 
  ind(dbIndex), idf(idfValue) {
  double qNorm = 0, tmpval;

  startIteration();
  while (hasMore()) {
    lemur::api::QueryTerm *qt = nextTerm();
    tmpval = qt->weight()*idf[qt->id()];
    setCount(qt->id(), tmpval);
    qNorm += tmpval * tmpval;
    delete qt;
  }
  qNorm = sqrt(qNorm);
  startIteration();
  while (hasMore()) {
    lemur::api::QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight()/qNorm);
    delete qt;
  }
}

/// Construct a query rep from an existing document (given document id).
lemur::retrieval::CosSimQueryRep::CosSimQueryRep(lemur::api::DOCID_T docId, const lemur::api::Index &dbIndex, 
                                                 double *idfValue): 
  ArrayQueryRep (dbIndex.termCountUnique() + 1), 
  ind(dbIndex), idf(idfValue) {
  double qNorm = 0;
  double tmpval;
  lemur::api::COUNT_T cnt;
  lemur::api::TERMID_T id;
  lemur::api::TermInfoList *tList = dbIndex.termInfoList(docId);
  lemur::api::TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    id = info->termID();
    cnt = info->count();
    tmpval = cnt * idf[id];
    setCount(id, tmpval);
    qNorm += tmpval * tmpval;
  }
  delete tList;
  qNorm = sqrt(qNorm);
  startIteration();
  while (hasMore()) {
    lemur::api::QueryTerm *qt = nextTerm();
    setCount(qt->id(), qt->weight()/qNorm);
    delete qt;
  }
}

lemur::retrieval::CosSimRetMethod::CosSimRetMethod(const lemur::api::Index &dbIndex, 
                                                   lemur::api::ScoreAccumulator &accumulator) :
  lemur::api::TextQueryRetMethod(dbIndex, accumulator), 
  L2FileName(CosSimParameter::defaultL2File) {
  fbParam.howManyTerms = CosSimParameter::defaultHowManyTerms;
  fbParam.posCoeff = CosSimParameter::defaultPosCoeff;
  // pre-compute IDF values
  idfV = new double[dbIndex.termCountUnique()+1];
  for (lemur::api::TERMID_T i=1; i<=dbIndex.termCountUnique(); i++) {
    idfV[i] = log((dbIndex.docCount()+1)/(0.5+dbIndex.docCount(i)));
  }
  loadDocNorms();  
  scFunc = new CosSimScoreFunc(dbIndex);
}

lemur::retrieval::CosSimRetMethod::CosSimRetMethod(const lemur::api::Index &dbIndex, const string &L2file,
                                                   lemur::api::ScoreAccumulator &accumulator) :
  lemur::api::TextQueryRetMethod(dbIndex, accumulator), L2FileName(L2file) {
  fbParam.howManyTerms = CosSimParameter::defaultHowManyTerms;
  fbParam.posCoeff = CosSimParameter::defaultPosCoeff;

  // pre-compute IDF values
  idfV = new double[dbIndex.termCountUnique()+1];
  for (lemur::api::TERMID_T i=1; i<=dbIndex.termCountUnique(); i++) {
    idfV[i] = log((dbIndex.docCount()+1)/(0.5+dbIndex.docCount(i)));
  }
  loadDocNorms();  
  scFunc = new CosSimScoreFunc(dbIndex);
}

void lemur::retrieval::CosSimRetMethod::loadDocNorms() {
  lemur::api::COUNT_T  numDocs = ind.docCount();
  docNorms = new double[numDocs + 1];
  for (lemur::api::COUNT_T j = 0; j <= numDocs; j++) {
    docNorms[j] = 0;
  }
  ifstream ifs;
  ifs.open(L2FileName.c_str());
  if (ifs.fail()) {
    cerr << "open file, wrong name:" << L2FileName << endl; 
    return;
  }
  for (lemur::api::COUNT_T i = 1; i <= numDocs; i++) {
    lemur::api::DOCID_T id;
    double norm;
    ifs >> id >> norm;
    if (id != i) {
      cerr << "alignment error in support file, wrong id:" << id << endl; 
      exit(1);
    }
    if (norm == 0) norm = 1;
    docNorms[i] = norm;
  }
  ifs.close();
}

lemur::retrieval::CosSimRetMethod::~CosSimRetMethod() {
  delete[](idfV); 
  delete[](docNorms);  
  delete(scFunc);
}

double lemur::retrieval::CosSimRetMethod::docNorm(lemur::api::DOCID_T docID) {
  // cache norms for docs already seen once.
  if (docNorms[docID] == 0) {
    lemur::api::TermInfoList *qList = ind.termInfoList(docID);
    lemur::api::TermInfo *qInfo;
    qList->startIteration();
    lemur::api::TERMID_T idx;
    lemur::api::COUNT_T dtf;
    double norm = 0, tmp;
    while (qList->hasMore()) {
      qInfo = qList->nextEntry();
      idx = qInfo->termID();
      dtf = qInfo->count();
      tmp = dtf * idfV[idx];
      norm += tmp * tmp;
    }
    delete qList;
    norm = sqrt(norm);
    if (norm == 0) norm = 1;
    docNorms[docID] = norm;    
  } 
  return docNorms[docID];
}

/// Use same as TFIDFRetMethod
void lemur::retrieval::CosSimRetMethod::updateTextQuery(lemur::api::TextQueryRep &qryRep, 
                                                        const lemur::api::DocIDSet &relDocs)
{
  lemur::api::COUNT_T totalTerm=ind.termCountUnique();  
  float * centroidVector = new float[totalTerm+1]; // one extra for OOV

  lemur::api::COUNT_T i;
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

    lemur::api::TermInfoList *tList = ind.termInfoList(docID);
    tList->startIteration();
    while (tList->hasMore()) {
      lemur::api::TermInfo *info = tList->nextEntry();
      centroidVector[info->termID()] += info->count(); // raw TF
    }
    delete tList;
  }

  // adjust term weight to obtain true Rocchio weight
  for (i=1; i<= totalTerm; i++) {
    if (centroidVector[i] >0 ) {
      centroidVector[i] *= idfV[i]/actualDocs;
    }
  }

  lemur::api::IndexedRealVector centVector(0);
  
  for (i=1; i< totalTerm; i++) {
    if (centroidVector[i]>0) {
      lemur::api::IndexedReal entry;
      entry.ind = i;
      entry.val = centroidVector[i];
      centVector.push_back(entry);
    }
  }
  centVector.Sort();
  lemur::api::IndexedRealVector::iterator j;
  lemur::api::COUNT_T termCount=0;
  for (j= centVector.begin();j!=centVector.end();j++) {
    if (termCount++ >= fbParam.howManyTerms) {
      break;
    } else {
      // add the term to the query vector
      (dynamic_cast<CosSimQueryRep *>(&qryRep))->incCount((*j).ind, (*j).val*fbParam.posCoeff);
    }
  }
  delete[](centroidVector);
}



