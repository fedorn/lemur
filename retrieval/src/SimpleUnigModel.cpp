#include "SimpleUnigModel.hpp"
#include <math.h>

void SimpleUnigramModel::Retrieve(double *query,
				  double querySum,
				  double *collectPr,
				  Index & dbIndex,
				  IndexedRealVector &scores,
				  SimpleDocUnigramModel &docModel,
				  double queryFidelity)
{
  int numDocs = dbIndex.docCount();

  int i;
  static double *scAcc = new double[numDocs];  
  // ASSUMING infinite score accumulators !!! 
  // XXX in the future we'll need to have a working set class

  static int *status = new int[numDocs]; 
  // status is needed to indicate which entry is a valid score
  // note that zero could be a valid score!


  for (i=0; i<numDocs; i++) {
    // scAcc[i]=log(docModel.unseenCoeff(i))*querySum;

    // new query noisy-channel model is
    scAcc[i]=log(queryFidelity*docModel.unseenCoeff(i)+
		 (1-queryFidelity))*querySum;
    status[i]=0;
  }

  int docCount;
  
  for (i=0; i< dbIndex.termCountUnique(); i++) {
    if (query[i]) {
      DocInfoList *dList = dbIndex.docInfoList(i);
      dList->startIteration();
      while (dList->hasMore()) {
	DocInfo *info = dList->nextEntry();
	// new query noisy-channel model is:
	double tmp1 = (1-queryFidelity)*collectPr[i];
	scAcc[info->docID()] += query[i]
	  *log((queryFidelity*docModel.seenProb(info->termCount(), i, info->docID())
		+ tmp1)/
	       (queryFidelity*docModel.unseenCoeff(info->docID())*collectPr[i]
		+tmp1));
	status[info->docID()]=1;
      }
    }
  }

  scores.clear();
  
  for (i=0; i< numDocs; i++) {
    if (status[i]) { // a valid entry
      // copy scores 
      IndexedReal entry;
      entry.ind = i;
      entry.val = scAcc[i];
      scores.push_back(entry);
    }
  }
}















