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


#include "OneStepMarkovChain.hpp"

lemur::langmod::OneStepMarkovChain::OneStepMarkovChain(const lemur::utility::WeightedIDSet &docSet, const lemur::api::Index &homeIndex, double *wdNorm, double stopProb):
  ind(homeIndex), alpha(stopProb),curToWord(0), norm(wdNorm)
{
  dSet = new int[ind.docCount()+1];
  //  norm = new double[ind.termCountUnique()+1];
  fromWordPr = new double[ind.termCountUnique()+1];

  int i;
  for (i=1; i<=ind.docCount(); i++) {
    dSet[i] = 0;
  }

  /*
    for (i=1; i<=ind.termCountUnique(); i++) {
    norm[i] = 0;
    }
  */

  // compute normalizer for each word, not an efficient way.
  // In the future, they should be pre-computed, e.g., in GenerateSmoothSupport.
  /*
    for (i=1; i<= ind.docCount(); i++) {
    TermInfoList *tList = ind.termInfoList(i);
    tList->startIteration();
    while (tList->hasMore()) {
    TermInfo *info = tList->nextEntry();
    norm[info->id()] += info->count()/(double)ind.docLength(i);
    }
    delete tList;
    }
  */
  docSet.startIteration();
  while (docSet.hasMore()) {
    int id;
    double pr;
    docSet.nextIDInfo(id,pr);
    dSet[id] = 1; //indicating presence in the doc set
  }
}

lemur::langmod::OneStepMarkovChain::~OneStepMarkovChain()
{
  delete [] fromWordPr;
  //  delete [] norm;
  delete [] dSet;
}

void lemur::langmod::OneStepMarkovChain::computeFromWordProb(lemur::api::TERMID_T toWord) const 
{
  lemur::api::COUNT_T i;
  for (i=1; i<=ind.termCountUnique(); i++) {
    fromWordPr[i] = 0;
  }  
  
  lemur::api::DocInfoList *dList = ind.docInfoList(toWord);
  dList->startIteration();
  while (dList->hasMore()) {
    const lemur::api::DocInfo *dinfo = dList->nextEntry();
    if (dSet[dinfo->docID()]==0) {
      continue;
    }
    double dLength = ind.docLength(dinfo->docID());
    // compute p(q_i|d)
    double p_qi_given_d = dinfo->termCount()/dLength;
   
    lemur::api::TermInfoList *tList = ind.termInfoList(dinfo->docID());
    const lemur::api::TermInfo *tinfo;
    tList->startIteration();
    while (tList->hasMore()) {
      tinfo = tList->nextEntry();
      // accumulate p(q_i|d)*p(w|d)
      fromWordPr[tinfo->termID()] += p_qi_given_d * tinfo->count()/dLength;
    }
    delete tList;
  }
  delete dList;

  // now normalize and adjust based on alpha
  for (i=1; i<=ind.termCountUnique(); i++) {
    if (norm[i]>0) {
      fromWordPr[i] = (1-alpha)*fromWordPr[i]/norm[i];
    } else { 
      fromWordPr[i] = 0;
    }
    if (toWord == i) {
      fromWordPr[i] += alpha;
      //cout << "diag: "<< fromWordPr[i] << endl;
    }
  }

  curToWord = toWord;

}

void lemur::langmod::OneStepMarkovChain::startFromWordIteration(lemur::api::TERMID_T toWord) const
{
  if (toWord != curToWord) 
    computeFromWordProb(toWord);

  itPos = 1;
}

void lemur::langmod::OneStepMarkovChain::nextFromWordProb(lemur::api::TERMID_T &fromWord, double &prob) const
{
  fromWord = itPos;
  prob = fromWordPr[itPos];
  itPos++;
}


