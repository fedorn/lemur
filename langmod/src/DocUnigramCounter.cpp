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


#include "DocUnigramCounter.hpp"


DocUnigramCounter::DocUnigramCounter(int docID, const Index &homeIndex)
  : ind(homeIndex), ArrayCounter<int>(homeIndex.termCountUnique()+1) 
{
  countDocUnigram(docID);
}

DocUnigramCounter::DocUnigramCounter(const vector<int> &docSet, const Index &homeIndex) 
  : ind(homeIndex), ArrayCounter<int>(homeIndex.termCountUnique()+1) 
{/*
  vector<int>::iterator it;
  for (it=docSet.begin(); it!= docSet.end(); it++) {
    countDocUnigram(*it);
    }*/
  for (int it=0;it<docSet.size();it++) {
    countDocUnigram(docSet[it]);
  }
}


DocUnigramCounter::DocUnigramCounter(const WeightedIDSet &docSet, const Index &homeIndex) 
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


DocUnigramCounter::DocUnigramCounter(const Index &collectionIndex)
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
  const TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    incCount(info->termID(), weight*info->count());
  }
  delete tList;
}



