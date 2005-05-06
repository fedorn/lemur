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

#include "FreqVector.hpp"

lemur::utility::HashFreqVector::HashFreqVector(const lemur::api::Index &index, lemur::api::DOCID_T docID) 
  : CSet<FreqCount, int>(300)
{
  lemur::api::TermInfoList *tList = index.termInfoList(docID);
  lemur::api::TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    FreqCount ct;
    ct.key = info->termID();
    add(ct, info->count());
  }
  delete tList;
}


bool lemur::utility::HashFreqVector::find(lemur::api::TERMID_T ind, int &freq) const 
{
  FreqCount c;
  c.key = ind;
  freq = count(c);
  if (freq==0) return false;
  return true;
}


void lemur::utility::HashFreqVector::nextFreq(lemur::api::TERMID_T &id, int &freq) const 
{
  // get the i-th element
  FreqCount c;
  id = (ISet<FreqCount>::operator[](i)).key;
  c.key = id;
  freq = count(c);
  i++;
}
