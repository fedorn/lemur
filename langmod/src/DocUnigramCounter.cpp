#include "DocUnigramCounter.hpp"


DocUnigramCounter::DocUnigramCounter(int docID, Index &homeIndex)
  : ind(homeIndex), ArrayCounter<int>(homeIndex.termCountUnique()+1) 
{
  countDocUnigram(docID);
}

DocUnigramCounter::DocUnigramCounter(vector<int> &docSet, Index &homeIndex) 
  : ind(homeIndex), ArrayCounter<int>(homeIndex.termCountUnique()+1) 
{
  vector<int>::iterator it;
  for (it=docSet.begin(); it!= docSet.end(); it++) {
    countDocUnigram(*it);
  }
}


DocUnigramCounter::DocUnigramCounter(WeightedIDSet &docSet, Index &homeIndex) 
  : ind(homeIndex), ArrayCounter<int>(homeIndex.termCountUnique()+1) 
{
  docSet.startIteration();
  while (docSet.hasMore()) {
    int docID;
    double wt;
    docSet.nextIDInfo(docID, wt);
    countDocUnigram(docID, wt);
  }
}


DocUnigramCounter::DocUnigramCounter(Index &collectionIndex)
  : ind(collectionIndex), ArrayCounter<int>(collectionIndex.termCountUnique()+1) 
{
  int numTerms = ind.termCountUnique();
  for (int i=1; i<= numTerms; i++) {
    incCount(i, ind.termCount(i));
  }
}

void DocUnigramCounter::countDocUnigram(int docID, double weight)
{
  TermInfoList *tList = ind.termInfoList(docID);
  TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    incCount(info->id(), weight*info->count());
  }
  delete tList;
}



