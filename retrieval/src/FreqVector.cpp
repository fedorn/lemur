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

#include "FreqVector.hpp"

HashFreqVector::HashFreqVector(const Index &index, DOCID_T docID) 
  : CSet<FreqCount, int>(300)
{
  TermInfoList *tList = index.termInfoList(docID);
  TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    static FreqCount ct;
    ct.key = info->termID();
    add(ct, info->count());
  }
  delete tList;
}


bool HashFreqVector::find(TERMID_T ind, int &freq) const 
{
  static FreqCount c;
  c.key = ind;
  freq = count(c);
  if (freq==0) return false;
  return true;
}


void HashFreqVector::nextFreq(TERMID_T &id, int &freq) const 
{
  // get the i-th element
  static FreqCount c;
  id = (ISet<FreqCount>::operator[](i)).key;
  c.key = id;
  freq = count(c)    ;
  i++;
}
