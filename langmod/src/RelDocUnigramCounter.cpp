/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  author: fff
*/
#include "RelDocUnigramCounter.hpp"


RelDocUnigramCounter::RelDocUnigramCounter(int docID, const Index &homeIndex)
  : ind(homeIndex), ArrayCounter<double>(homeIndex.termCountUnique()+1) 
{
  countRelDocUnigram(docID);
}

RelDocUnigramCounter::RelDocUnigramCounter(const WeightedIDSet &docSet, const Index &homeIndex) 
  : ind(homeIndex), ArrayCounter<double>(homeIndex.termCountUnique()+1) 
{
  docSet.startIteration();
  while (docSet.hasMore()) {
    int docID;
    double wt;
    docSet.nextIDInfo(docID, wt);
    countRelDocUnigram(docID, wt);
  }
}

void RelDocUnigramCounter::countRelDocUnigram(int docID, double weight)
{
  double dlength = (double)ind.docLength(docID);
  TermInfoList *tList = ind.termInfoList(docID);
  TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    incCount(info->termID(), weight*((double)info->count()/dlength));
  }
  delete tList;
}
