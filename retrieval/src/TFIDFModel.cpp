#include "TFIDFModel.hpp"

TFIDFModel::WeightParam TFIDFModel::defaultQTF(TFIDFModel::BM25, 1, 0.8,3);
TFIDFModel::WeightParam TFIDFModel::defaultDTF(TFIDFModel::BM25, 1, 0.8, 1000);


// assuming query is already weighted!

void TFIDFModel::computeFeedbackQuery(double *query, 
				      IndexedRealVector * topScores,
				      Index &dbIndex,
				      const WeightParam &docparam,
				      int howManyDocs,
				      int howManyTerms,
				      double posCoeff)
{

  int totalTerm=dbIndex.termCountUnique();  
  static float * centroidVector = new float[totalTerm];


  IndexedRealVector::iterator j;
  int count=0;
  int docLength;
  int actualDocs=0;
  int i;
  for (i=0;i<totalTerm;i++) {
    centroidVector[i]=0;
  }
  for (j= topScores->begin();j!=topScores->end();j++) {
    if (count++ >= howManyDocs) {
      break;
    } else {
      actualDocs++;
      TermInfoList *tList = dbIndex.termInfoList((*j).ind);
      tList->startIteration();
      while (tList->hasMore()) {
	TermInfo *info = tList->nextEntry();
	centroidVector[info->id()] += DocTFWeight(info->count(), (*j).ind, 
						  dbIndex,docparam);
      }
    }
  }

  // adjust term weight to obtain true Rocchio weight
  for (i=0; i< totalTerm; i++) {
    if (centroidVector[i] >0 ) {
      centroidVector[i] *= idf(dbIndex.docCount(), dbIndex.docCount(i))/actualDocs;
    }
  }

  IndexedRealVector centVector(0);
  
  for (i=0; i< totalTerm; i++) {
    if (centroidVector[i]>0) {
      IndexedReal entry;
      entry.ind = i;
      entry.val = centroidVector[i];
      centVector.push_back(entry);
    }
  }
  
  centVector.Sort();

  int termCount=0;
  for (j= centVector.begin();j!=centVector.end();j++) {
    if (termCount++ >= howManyTerms) {
      break;
    } else {
      // add the term to the query vector
      query[(*j).ind] += (*j).val*posCoeff;
    }
  }

}





// XXX SIDE EFFECT: query has weights after this function is called,
//             even  if it originally was not weighted.

void TFIDFModel::retrieve(double *query,
			  double querySum,
			  Index &dbIndex,
			  IndexedRealVector &scores,
			  const WeightParam &qryparam,
			  const WeightParam &docparam,
			  bool isWeightedQuery)
{


  int numDocs = dbIndex.docCount();

  int i;
  static double *scAcc = new double[numDocs+1];  
  // ASSUMING max number of  score accumulators !!! 
  // one more elements needed to accommodate OOV id

  static int *status = new int[numDocs]; 
  // status is needed to indicate which entry is a valid score
  // note that zero could be a valid score!


  for (i=0; i<=numDocs; i++) {
    scAcc[i]=0;
    status[i]=0;
  }


  
  for (i=0; i< dbIndex.termCountUnique(); i++) {
    if (query[i]) {
      double idfVal = idf(numDocs, dbIndex.docCount(i));



      // set query term weight, in case Rocchio feedback is to be done.
      if (!isWeightedQuery) {
	query[i] = QueryTFWeight(query[i], querySum, qryparam)*idfVal;
      }


      DocInfoList *dList = dbIndex.docInfoList(i);
      dList->startIteration();
      while (dList->hasMore()) {
	
	DocInfo *info = dList->nextEntry();
	
       	scAcc[info->docID()] += query[i] *
	  idfVal * DocTFWeight(info->termCount(),info->docID(), dbIndex, docparam);
	status[info->docID()]=1;
      }
    }
  }

  scores.clear();
  
  for (i=0; i<= numDocs; i++) {
    if (status[i]) { // a valid entry
      // copy scores 
      IndexedReal entry;
      entry.ind = i;
      entry.val = scAcc[i];
      scores.push_back(entry);
    }
  }

}



void TFIDFModel::retrieveWorkSet(double *queryArray,
				 double querySum,
				 WorkingSet &workSet,
				 Index &dbIndex,
				 IndexedRealVector &scores,
				 const WeightParam &qryparam,
				 const WeightParam &docparam,
				 bool isWeightedQuery)
{

  int numTerms = dbIndex.termCountUnique();
  // compute idf first
  static float *idfArray = new float[numTerms];


  int i;
  for (i=0; i<numTerms; i++) {
    if (queryArray[i]) {
      idfArray[i] = idf(dbIndex.docCount(), dbIndex.docCount(i));
      if (!isWeightedQuery ) {
	queryArray[i] = idfArray[i]*
	  QueryTFWeight(queryArray[i],querySum, qryparam);
      }
    }
  }
  

  // score each doc
  scores.clear();
  WorkingSet::iterator it;
  for (it=workSet.begin(); it!=workSet.end(); it++) {
    IndexedReal entry;
    entry.ind = workSet[it];
    entry.val = 0;
    TermInfoList *tList = dbIndex.termInfoList(workSet[it]);
    tList->startIteration();
    while (tList->hasMore()) {
      TermInfo *info = tList->nextEntry();
      
      entry.val += DocTFWeight(info->count(), workSet[it], dbIndex,docparam)*
	idfArray[info->id()]* queryArray[info->id()];

    }
    scores.push_back(entry);
  }
}
  


double TFIDFModel::DocTFWeight(const double rawTF, const int docID, Index & dbIndex, const WeightParam &param)
{
  if (param.tf == RAWTF) {
    return (rawTF);
  } else if (param.tf == LOGTF) {
    return (log(rawTF+1));
  } else if (param.tf == BM25) {
    
    return (BM25TF(rawTF, param.bm25K1, param.bm25B,
		   dbIndex.docLength(docID),param.bm25AvgLen));
  } else {  // default to raw TF
    cerr << "Warning: unknown TF method, raw TF assumed\n";
    return rawTF;
  }
}


double TFIDFModel::QueryTFWeight(const double rawTF, const double qryLength, const WeightParam &param)
{
  if (param.tf == RAWTF) {
    return (rawTF);
  } else if (param.tf == LOGTF) {
    return (log(rawTF+1));
  } else if (param.tf == BM25) {
    return (BM25TF(rawTF, param.bm25K1, param.bm25B,
		   qryLength,param.bm25AvgLen));
  } else {  // default to raw TF
    cerr << "Warning: unknown TF method, raw TF assumed\n";
    return rawTF;
  }
}






