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
  author: fff
*/
#include "RelDocUnigramCounter.hpp"


lemur::langmod::RelDocUnigramCounter::RelDocUnigramCounter(lemur::api::DOCID_T docID, const lemur::api::Index &homeIndex)
  : ind(homeIndex), lemur::utility::ArrayCounter<double>(homeIndex.termCountUnique()+1) 
{
  countRelDocUnigram(docID);
}

lemur::langmod::RelDocUnigramCounter::RelDocUnigramCounter(const lemur::utility::WeightedIDSet &docSet, const lemur::api::Index &homeIndex) 
  : ind(homeIndex), lemur::utility::ArrayCounter<double>(homeIndex.termCountUnique()+1) 
{
  docSet.startIteration();
  while (docSet.hasMore()) {
    int docID;
    double wt;
    docSet.nextIDInfo(docID, wt);
    countRelDocUnigram(docID, wt);
  }
}

void lemur::langmod::RelDocUnigramCounter::countRelDocUnigram(lemur::api::DOCID_T docID, double weight)
{
  double dlength = (double)ind.docLength(docID);
  lemur::api::TermInfoList *tList = ind.termInfoList(docID);
  lemur::api::TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    incCount(info->termID(), weight*((double)info->count()/dlength));
  }
  delete tList;
}
