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

#include "FloatFreqVector.hpp"

FloatFreqVector::FloatFreqVector(const Index &index, int docID) 
  : CSet<FreqCounter, double>(1000)
{
  TermInfoList *tList = index.termInfoList(docID);
  TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    FreqCounter ct;
    ct.key = info->termID();
    add(ct, info->count());
  }
  delete tList;
  s2 = 0;
}

FloatFreqVector::FloatFreqVector(const Index &index, TermInfoList *tList) 
  : CSet<FreqCounter, double>(1000)
{
  TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    FreqCounter ct;
    ct.key = info->termID();
    add(ct, info->count());
  }
  s2 = 0;
}

FloatFreqVector::FloatFreqVector(const Index &index, vector<int> &dids) 
  : CSet<FreqCounter, double>(1000)
{
  for (int j = 0; j < dids.size(); j++) {
    int did = dids[j];
    TermInfoList *tList = index.termInfoList(did);
    TermInfo *info;
    tList->startIteration();
    while (tList->hasMore()) {
      info = tList->nextEntry();
      FreqCounter ct;
      ct.key = info->termID();
      add(ct, info->count());
    }
    delete tList;
  }
  s2 = 0;
}

FloatFreqVector::FloatFreqVector(FloatFreqVector *v2) 
  : CSet<FreqCounter, double>(1000)
{
  double tmp;
  int idx;
  FreqCounter c;
  v2->startIteration();
  while(v2->hasMore()) {
    v2->nextFreq(idx, tmp);
    c.key = idx;
    add(c, tmp);
  }
  s2 = 0;
}

void FloatFreqVector::addVal(int id, int cnt) {
  FreqCounter ct;
  ct.key = id;
  add(ct, cnt);
}

void FloatFreqVector::addVal(int id, double freq) {
  FreqCounter ct;
  ct.key = id;
  add(ct, freq);
}

bool FloatFreqVector::find(int ind, double &freq) const
{
  FreqCounter c;
  c.key = ind;
  freq = count(c);
  if (freq==0) return false;
  return true;
}

bool FloatFreqVector::find(int ind, int &freq) const
{
  double tmp;
  bool ret = find(ind, tmp);
  freq = (int) tmp;
  return ret;
}

void FloatFreqVector::nextFreq(int &id, int &freq)  const {
  double tmp;
  int idx;
  nextFreq(idx, tmp);
  id = idx;
  freq = (int) tmp;
}

void FloatFreqVector::nextFreq(int &id, double &freq) const
{
  // get the i-th element
  FreqCounter c;
  id = (ISet<FreqCounter>::operator[](i)).key;
  c.key = id;
  freq = count(c)    ;
  i++;
}

double FloatFreqVector::sum() const {
  double ret = 0, tmp;
  int idx;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    ret += tmp;
  }
  return ret;
}

double FloatFreqVector::sum2() const {
  if (s2 != 0) return s2;
  double ret = 0, tmp;
  int idx;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    ret += tmp * tmp;
  }
  s2 = ret;
  return ret;
}

void FloatFreqVector::weigh(const double *vals) {
  double tmp;
  int idx;
  FreqCounter c;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    c.key = idx;
    tmp *= vals[idx];
    setCount(c, tmp);
  }
  s2 = 0;
}

void FloatFreqVector::weigh(double val) {
  double tmp;
  int idx;
  FreqCounter c;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    c.key = idx;
    tmp *= val;
    setCount(c, tmp);
  }
  s2 = 0;
}

void FloatFreqVector::addVec(const FloatFreqVector *v2) {
  double tmp;
  int idx;
  FreqCounter c;
  v2->startIteration();
  while(v2->hasMore()) {
    v2->nextFreq(idx, tmp);
    c.key = idx;
    add(c, tmp);
  }
  s2 = 0;
}

void FloatFreqVector::subtract(const FloatFreqVector *v2) {
  double tmp;
  int idx;
  FreqCounter c;
  v2->startIteration();
  while(v2->hasMore()) {
    v2->nextFreq(idx, tmp);
    c.key = idx;
    add(c, -tmp);
  }
  s2 = 0;
}

double FloatFreqVector::dotProd(const FloatFreqVector *v2) {
  double tmp1, tmp2, ret = 0;  
  int idx;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp1);
    if (v2->find(idx, tmp2)) {
      ret += tmp1 * tmp2;
    }
  }
  return ret;
}
