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


#include "DocUnigramCounter.hpp"


lemur::langmod::DocUnigramCounter::DocUnigramCounter(lemur::api::DOCID_T docID, const lemur::api::Index &homeIndex)
  : ind(homeIndex), lemur::utility::ArrayCounter<int>(homeIndex.termCountUnique()+1) 
{
  countDocUnigram(docID);
}

lemur::langmod::DocUnigramCounter::DocUnigramCounter(const vector<lemur::api::DOCID_T> &docSet, const lemur::api::Index &homeIndex) 
  : ind(homeIndex), lemur::utility::ArrayCounter<int>(homeIndex.termCountUnique()+1) {
  for (int it=0;it<docSet.size();it++) {
    countDocUnigram(docSet[it]);
  }
}


lemur::langmod::DocUnigramCounter::DocUnigramCounter(const lemur::utility::WeightedIDSet &docSet, const lemur::api::Index &homeIndex) 
  : ind(homeIndex), lemur::utility::ArrayCounter<int>(homeIndex.termCountUnique()+1) 
{
  docSet.startIteration();
  while (docSet.hasMore()) {
    int docID;
    double wt;
    docSet.nextIDInfo(docID, wt);
    countDocUnigram(docID, wt);
  }
}


lemur::langmod::DocUnigramCounter::DocUnigramCounter(const lemur::api::Index &collectionIndex)
  : ind(collectionIndex), lemur::utility::ArrayCounter<int>(collectionIndex.termCountUnique()+1) 
{
  lemur::api::COUNT_T numTerms = ind.termCountUnique();
  for (lemur::api::COUNT_T i=1; i<= numTerms; i++) {
    incCount(i, ind.termCount(i));
  }
}

void lemur::langmod::DocUnigramCounter::countDocUnigram(lemur::api::DOCID_T docID, double weight)
{
  lemur::api::TermInfoList *tList = ind.termInfoList(docID);
  const lemur::api::TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    incCount(info->termID(), weight*info->count());
  }
  delete tList;
}



