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

#include "FloatFreqVector.hpp"

lemur::utility::FloatFreqVector::FloatFreqVector(const lemur::api::Index &index, lemur::api::DOCID_T docID) 
  : CSet<FloatFreqCounter, double>(1000)
{
  lemur::api::TermInfoList *tList = index.termInfoList(docID);
  lemur::api::TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    FloatFreqCounter ct;
    ct.key = info->termID();
    add(ct, info->count());
  }
  delete tList;
  s2 = 0;
}

lemur::utility::FloatFreqVector::FloatFreqVector(const lemur::api::Index &index, lemur::api::TermInfoList *tList) 
  : CSet<FloatFreqCounter, double>(1000)
{
  lemur::api::TermInfo *info;
  tList->startIteration();
  while (tList->hasMore()) {
    info = tList->nextEntry();
    FloatFreqCounter ct;
    ct.key = info->termID();
    add(ct, info->count());
  }
  s2 = 0;
}

lemur::utility::FloatFreqVector::FloatFreqVector(const lemur::api::Index &index, vector<lemur::api::DOCID_T> &dids) 
  : CSet<FloatFreqCounter, double>(1000)
{
  for (unsigned int j = 0; j < dids.size(); j++) {
    lemur::api::DOCID_T did = dids[j];
    lemur::api::TermInfoList *tList = index.termInfoList(did);
    lemur::api::TermInfo *info;
    tList->startIteration();
    while (tList->hasMore()) {
      info = tList->nextEntry();
      FloatFreqCounter ct;
      ct.key = info->termID();
      add(ct, info->count());
    }
    delete tList;
  }
  s2 = 0;
}

lemur::utility::FloatFreqVector::FloatFreqVector(FloatFreqVector *v2) 
  : CSet<FloatFreqCounter, double>(1000)
{
  double tmp;
  lemur::api::TERMID_T idx;
  FloatFreqCounter c;
  v2->startIteration();
  while(v2->hasMore()) {
    v2->nextFreq(idx, tmp);
    c.key = idx;
    add(c, tmp);
  }
  s2 = 0;
}

void lemur::utility::FloatFreqVector::addVal(lemur::api::TERMID_T id, int cnt) {
  FloatFreqCounter ct;
  ct.key = id;
  add(ct, cnt);
}

void lemur::utility::FloatFreqVector::addVal(lemur::api::TERMID_T id, double freq) {
  FloatFreqCounter ct;
  ct.key = id;
  add(ct, freq);
}

bool lemur::utility::FloatFreqVector::find(lemur::api::TERMID_T ind, double &freq) const
{
  FloatFreqCounter c;
  c.key = ind;
  freq = count(c);
  if (freq==0) return false;
  return true;
}

bool lemur::utility::FloatFreqVector::find(lemur::api::TERMID_T ind, int &freq) const
{
  double tmp;
  bool ret = find(ind, tmp);
  freq = (int) tmp;
  return ret;
}

void lemur::utility::FloatFreqVector::nextFreq(lemur::api::TERMID_T &id, int &freq)  const {
  double tmp;
  lemur::api::TERMID_T idx;
  nextFreq(idx, tmp);
  id = idx;
  freq = (int) tmp;
}

void lemur::utility::FloatFreqVector::nextFreq(lemur::api::TERMID_T &id, double &freq) const
{
  // get the i-th element
  FloatFreqCounter c;
  id = (ISet<FloatFreqCounter>::operator[](i)).key;
  c.key = id;
  freq = count(c)    ;
  i++;
}

double lemur::utility::FloatFreqVector::sum() const {
  double ret = 0, tmp;
  lemur::api::TERMID_T idx;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    ret += tmp;
  }
  return ret;
}

double lemur::utility::FloatFreqVector::sum2() const {
  if (s2 != 0) return s2;
  double ret = 0, tmp;
  lemur::api::TERMID_T idx;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    ret += tmp * tmp;
  }
  s2 = ret;
  return ret;
}

void lemur::utility::FloatFreqVector::weigh(const double *vals) {
  double tmp;
  lemur::api::TERMID_T idx;
  FloatFreqCounter c;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    c.key = idx;
    tmp *= vals[idx];
    setCount(c, tmp);
  }
  s2 = 0;
}

void lemur::utility::FloatFreqVector::weigh(double val) {
  double tmp;
  lemur::api::TERMID_T idx;
  FloatFreqCounter c;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp);
    c.key = idx;
    tmp *= val;
    setCount(c, tmp);
  }
  s2 = 0;
}

void lemur::utility::FloatFreqVector::addVec(const FloatFreqVector *v2) {
  double tmp;
  lemur::api::TERMID_T idx;
  FloatFreqCounter c;
  v2->startIteration();
  while(v2->hasMore()) {
    v2->nextFreq(idx, tmp);
    c.key = idx;
    add(c, tmp);
  }
  s2 = 0;
}

void lemur::utility::FloatFreqVector::subtract(const FloatFreqVector *v2) {
  double tmp;
  lemur::api::TERMID_T idx;
  FloatFreqCounter c;
  v2->startIteration();
  while(v2->hasMore()) {
    v2->nextFreq(idx, tmp);
    c.key = idx;
    add(c, -tmp);
  }
  s2 = 0;
}

double lemur::utility::FloatFreqVector::dotProd(const FloatFreqVector *v2) {
  double tmp1, tmp2, ret = 0;  
  lemur::api::TERMID_T idx;
  startIteration();
  while(hasMore()) {
    nextFreq(idx, tmp1);
    if (v2->find(idx, tmp2)) {
      ret += tmp1 * tmp2;
    }
  }
  return ret;
}
