/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#include <iomanip>
#include "Summarizer.hpp"
#include "Passage.hpp"
#include "BasicPassage.hpp"
#include "InvFPIndex.hpp"
#include <vector>

using std::vector;

#ifndef _BASICSUMM_HPP
#define _BASICSUMM_HPP

#define EOS      "*eos"
#define PSG_LEN  15

/*!
  A simple summarizer implementation that uses sentence selection to create summaries.  This example shows how a simple summarizer may look, and how to use the <code>Summarizer</code> abstract with the <code>Passage</code> abstract.  Note that this implementation is paired with an implementation of <code>Passage</code> called <code>BasicPassage</code>.

*/
class BasicSumm : public Summarizer {

private:
  InvFPIndex* idx;
  int summLen;
  vector<BasicPassage> doc;
  int iterCount;

public:
  /// Constructor takes index information and an option default summary length
  BasicSumm(InvFPIndex* inIdx, int inSummLen = 5) {
    idx = inIdx;
    summLen = inSummLen;
    iterCount = 1;
  };

  virtual void summDocument(const char* docID, const int optLen = -1, const char* qInfo = NULL);

  virtual void scorePassages(const char* qInfo = NULL);

  virtual void markPassages(int optLen = -1, char* qInfo = NULL);

  virtual void addPassage(Passage &psg);

  virtual void clear(void);

  virtual int fetchPassages(Passage psgs[], int optLen = -1);

  virtual int nextPassage(Passage* psg);

  virtual void iterClear(void);

  virtual void outputSumm(void);

  /// Checks for EOS marker
  int isEOS(const char* check) {
    return !strcmp(check, EOS);
  }

  /// Determines if any EOS markers are present
  int hasEOS(InvFPIndex* idx, TermInfoList* tList) {
    tList->startIteration();
    TermInfo* tEntry;
    while (tList->hasMore()) {
      tEntry = tList->nextEntry();
      if ( isEOS(idx->term(tEntry->id())) ) return true;
    }
    return false;
  }

  /// Scores an individual passage
  double scorePassage(BasicPassage &psg, char* qInfo = NULL) {
    char* docID = psg.docID;
    passageVec psgV= *psg.getAsVector();
    double psgLen = psgV.size();
    double P = 1;  // no markup yet, all get same weight
    double M = 1.5;
    double endScore, Tf, tf, idf, docLen, avgDocLen;
    endScore = 0.0;
    for (int i=0; i < psgLen; i++) {
      docLen = idx->docLength(idx->document(docID));
      avgDocLen = idx->docLengthAvg();
      tf = psgV[i].tf;
      Tf = tf / (tf + 0.5 + 1.5 * (docLen/avgDocLen) );
      //cout << "score1: " << log((double)idx->docCount()/(double)idx->docCount(psgV[i].termID)) << endl;
      idf = min(M, log((double)idx->docCount()/(double)idx->docCount(psgV[i].termID))); 
      endScore += (Tf * idf * P);
    }
    endScore = endScore / 1+psgLen;
    psg.score = endScore;
    //cout << "score2: " << endScore << endl;
    return endScore;
  }

  /// Locate the next passage in a document by searching for the next EOS or using a max length
  void findNextPassage(BasicPassage &psg, InvFPIndex* idx, 
		       TermInfoList* tList, int eos) {
    TermInfo* tEntry;
    psg.clear();
    termCount* storage;
    if (eos) {
      while (tList->hasMore()) {
	tEntry = tList->nextEntry();
	if ( isEOS(idx->term(tEntry->id())) ) return;
	storage = new termCount;
	storage->termID = tEntry->id();
	storage->tf = tEntry->count();
	psg.addTerm(*storage);
      }
    } else {
      for(int i=0; i < PSG_LEN; i++) {
	if (tList->hasMore()) {
	  tEntry = tList->nextEntry();
	  storage = new termCount;
	  storage->termID = tEntry->id();
	  storage->tf = tEntry->count();
	  psg.addTerm(*storage);
	} else {
	  return;
	}
      }
    }
    return;
  }
 
  /// <code>BasicSumm</code>'s method to output a summary (to screen)
  void showPassage(passageVec* psg, InvFPIndex* idx) {
    for (int i=0; i < psg->size(); i++) {
      cout << idx->term((*psg)[i].termID) << " ";
    }
  }

  /// Only display passages flagged as 'marked' as part of the output
  void showMarkedPassages() {
    
    for (int i=0; i<doc.size(); i++) {
      if (doc[i].marked > 0) {
	showPassage(doc[i].getAsVector(), idx);
	cout << endl;
      }
    }
  }

}; // BasicSumm

#endif
