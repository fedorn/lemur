#include "OkapiModel.hpp"

OkapiModel::Parameter OkapiModel::defaultParam(500);




void OkapiModel::retrieve(double *query, // query TF vector
			  Index & dbIndex, // database
			  IndexedRealVector &scores, // results
			  const Parameter &param)
{
  int numTerms = dbIndex.termCountUnique();
  int numDocs = dbIndex.docCount();
  
  // first, computer Robertson/Spark Jones weight
  static double * weightedQuery = new double[numTerms];
  int i;
  for (i=0; i< numTerms; i++) {
    if (query[i]) {
      weightedQuery[i] = RSJWeight(0,0, dbIndex.docCount(i),
				   numDocs) *
	(param.k3+1)*query[i]/(param.k3+query[i]);
    } else {
      weightedQuery[i]=0;
    }
  }
  scoreDocuments(weightedQuery, dbIndex, scores, param);
}


void OkapiModel::scoreDocuments(double *weightedQuery,
				Index &dbIndex,
				IndexedRealVector &scores,
				const Parameter &param)
{
  // score each document

  int numTerms = dbIndex.termCountUnique();

  int numDocs = dbIndex.docCount();
  int i;
  static double *docScores = new double[numDocs];
  static int *docMatched = new int[numDocs];
  for (i=0; i< numDocs; i++) {
    docScores[i]=0;
    docMatched[i]=0;
  }
  for (i=0; i< numTerms; i++) {
    if (weightedQuery[i]) {
      DocInfoList *dList = dbIndex.docInfoList(i);
      dList->startIteration();
      while (dList->hasMore()) {
	DocInfo *info = dList->nextEntry();
	docScores[info->docID()] += weightedQuery[i]*BM25TF(info->termCount(), dbIndex.docLength(info->docID()), param);
	docMatched[info->docID()]=1;
      }
    }
  }
  
  // pack scores;
  scores.clear();
  for (i=0; i< numDocs; i++) {
    if (docMatched[i]) {
      scores.PushValue(i, docScores[i]);
    }
  }
}

void OkapiModel::feedbackRetrieve(double *query, // query TF vector
				  IndexedRealVector  & scores,
				  Index &dbIndex,
				  const Parameter &param,
				  int howManyDocs,
				  int howManyTerms)
{


  int totalTerm=dbIndex.termCountUnique();  
  static int * relCounts = new int[totalTerm];


  int i;
  for (i=0;i<totalTerm;i++) {
    relCounts[i]=0;
  }

  IndexedRealVector::iterator j;
  int count=0;
  int docLength;
  int actualDocs=0;

  for (j= scores.begin();j!=scores.end();j++) {
    if (count++ >= howManyDocs) {
      break;
    } else {
      actualDocs++;
      TermInfoList *tList = dbIndex.termInfoList((*j).ind);
      tList->startIteration();
      while (tList->hasMore()) {
	TermInfo *info = tList->nextEntry();
	
	relCounts[info->id()] ++;
      }
    }
  }

  IndexedRealVector weightedTerms(0);
  
  // adjust term weight
  for (i=0; i< totalTerm; i++) {
    if (relCounts[i] >0 ) {
      weightedTerms.PushValue(i, relCounts[i]*
			      RSJWeight(relCounts[i],actualDocs, 
					dbIndex.docCount(i),
					dbIndex.docCount()));
    }
  }
  
  weightedTerms.Sort();

  static double * weightedQuery = new double[totalTerm];
  int termCount=0;
  for (j= weightedTerms.begin();j!=weightedTerms.end();j++) {
    if (termCount++ >= howManyTerms) {
      break;
    } else {
      // add the term to the query vector
      query[(*j).ind] += param.expQTF;
    } 
  }
  
  for (i=0; i< totalTerm; i++) {
    if (query[i]>0) {
      weightedQuery[i] = RSJWeight(relCounts[i],actualDocs, dbIndex.docCount(i),
				   dbIndex.docCount()) *
	(param.k3+1)*query[i]/(param.k3+query[i]);
    } else {
      weightedQuery[i]=0;
    }
  }
  scoreDocuments(weightedQuery, dbIndex, scores, param);

}













