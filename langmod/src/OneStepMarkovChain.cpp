 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "OneStepMarkovChain.hpp"

OneStepMarkovChain::OneStepMarkovChain(WeightedIDSet &docSet, Index &homeIndex, double stopProb):
  ind(homeIndex), alpha(stopProb),curToWord(0)
{
  dSet = new int[ind.docCount()+1];
  norm = new double[ind.termCountUnique()+1];
  fromWordPr = new double[ind.termCountUnique()+1];

  int i;
  for (i=1; i<=ind.docCount(); i++) {
    dSet[i] = 0;
  }

  for (i=1; i<=ind.termCountUnique(); i++) {
    norm[i] = 0;
  }

  docSet.startIteration();
  while (docSet.hasMore()) {
    int id;
    double pr;
    docSet.nextIDInfo(id,pr);
    dSet[id] = 1; //indicating presence in the doc set
    TermInfoList *tList = ind.termInfoList(id);
    TermInfo *info;
    tList->startIteration();
    while (tList->hasMore()) {
      info = tList->nextEntry();
      norm[info->id()] += info->count()/(double)ind.docLength(id);
    }
    delete tList;
  }
}

OneStepMarkovChain::~OneStepMarkovChain()
{
  delete [] fromWordPr;
  delete [] norm;
  delete [] dSet;
}

void OneStepMarkovChain::computeFromWordProb(int toWord)
{
  int i;
  for (i=1; i<=ind.termCountUnique(); i++) {
    fromWordPr[i] = 0;
  }  
  
  DocInfoList *dList = ind.docInfoList(toWord);
  dList->startIteration();
  while (dList->hasMore()) {
    DocInfo *dinfo = dList->nextEntry();
    if (dSet[dinfo->docID()]==0) {
      continue;
    }
    double dLength = ind.docLength(dinfo->docID());
    // compute p(q_i|d)
    double p_qi_given_d = dinfo->termCount()/dLength;
   
    TermInfoList *tList = ind.termInfoList(dinfo->docID());
    TermInfo *tinfo;
    tList->startIteration();
    while (tList->hasMore()) {
      tinfo = tList->nextEntry();
      // accumulate p(q_i|d)*p(w|d)
      fromWordPr[tinfo->id()] += p_qi_given_d * tinfo->count()/dLength;
    }
    delete tList;
  }
  delete dList;

  // now normalize and adjust based on alpha
  for (i=1; i<=ind.termCountUnique(); i++) {
    if (norm[i]>0) {
      fromWordPr[i] = (1-alpha)*fromWordPr[i]/norm[i];
    }
    if (toWord == i) {
      fromWordPr[i] += alpha;
      // cout << "diag: "<< fromWordPr[i] << endl;
    }
  }

  curToWord = toWord;

}

void OneStepMarkovChain::startFromWordIteration(int toWord)
{
  if (toWord != curToWord) 
    computeFromWordProb(toWord);

  itPos = 1;
}

void OneStepMarkovChain::nextFromWordProb(int &fromWord, double &prob) 
{
  fromWord = itPos;
  prob = fromWordPr[itPos];
   itPos++;
}


