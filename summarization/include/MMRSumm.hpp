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

#ifndef _MMRSUMM_HPP
#define _MMRSUMM_HPP

#include <iomanip>
#include "Summarizer.hpp"
#include "Passage.hpp"
#include "MMRPassage.hpp"
#include "InvFPIndex.hpp"
#include <algorithm>
#include <vector>

using std::vector;

#define EOS      "*eos"
#define TITLE    "*title"
#define PRONOUN  "*pronoun"
#define PSG_LEN  15

/*!
  A summarizer implementation that uses MMR to create summaries.  This example shows how a slightly more complex summarizer may look, and how to use the <code>Summarizer</code> abstract with the <code>Passage</code> abstract.  Note that this implementation is paired with an implementation of <code>Passage</code> called <code>MMRPassage</code>.

*/

class MMRSumm : public Summarizer {

private:
  double lambda;
  InvFPIndex* idx;
  int summLen;
  vector<MMRPassage> doc;
  int iterCount;
  double maxSims;
  MMRPassage* queryPassage;

  int autoMMRQuery(void) {
    TermInfo* tEntry;
    TermInfoList* tList = idx->termInfoListSeq(idx->document(queryPassage->docID));
    termCount* storage;
    if (hasTITLE(idx, tList)) {
      // use title words
      tList->startIteration();
      cout << "title found" << endl;
      while (tList->hasMore()) {
	tEntry = tList->nextEntry();
	if ( isTITLE(idx->term(tEntry->id())) ) {
	  tEntry = tList->nextEntry(); // the actual word after title token
	  storage = new termCount;
	  storage->termID = tEntry->id();
	  storage->tf = tEntry->count();
	  storage->val = tEntry->count(); // :TODO: fix
	  queryPassage->addTerm(*storage);
	}
      }      
    } else {
      // :TODO: replace with something smarter
      tList->startIteration();
      for (int i=0; i<10; i++) {
	if (tList->hasMore()) {
	  tEntry = tList->nextEntry();
	  storage = new termCount;
	  storage->termID = tEntry->id();
	  storage->tf = tEntry->count();
	  storage->val = tEntry->count(); // :TODO: fix
	  queryPassage->addTerm(*storage);
	}
      } 
    }
    cout << "Autoquery: ";
    showPassage((*queryPassage).getAsVector(), idx);
    cout << endl;

    return 1;
  }

  int setMMRQuery(char* qInfo) {
    //:TODO: 
    return autoMMRQuery();
  }

public:

  MMRSumm(InvFPIndex* inIdx, int inSummLen = 5) {
    idx = inIdx;
    summLen = inSummLen;
    iterCount = 1;
    maxSims = -1.0;
    queryPassage = NULL;
    lambda = 1.0;
  };
  
  //:TODO: undo qInfo here
  virtual void markPassages(int optLen, char* qInfo);

  virtual void addPassage(Passage &psg);

  virtual int fetchPassages(Passage psgs[], int optLen);
  
  virtual void summDocument(const char* docID, const int optLen, const char* qInfo);

  virtual void scorePassages(const char* qInfo);

  virtual void clear(void);

  virtual int nextPassage(Passage* psg);

  virtual void iterClear(void);

  virtual void outputSumm(void);

  void findNextPassage(MMRPassage &psg, InvFPIndex* idx, 
		       TermInfoList* tList, int eos);

  void showPassage(passageVec* psg, InvFPIndex* idx);
  
  void showMarkedPassages();

  int isEOS(const char* check) {
    return !strcmp(check, EOS);
  }
  
  int hasEOS(InvFPIndex* idx, TermInfoList* tList) {
    tList->startIteration();
    TermInfo* tEntry;
    while (tList->hasMore()) {
      tEntry = tList->nextEntry();
      if ( isEOS(idx->term(tEntry->id())) ) return true;
    }
    return false;
  }
  
  int isTITLE(const char* check) {
    return !strcmp(check, TITLE);
  }
  
  int hasTITLE(InvFPIndex* idx, TermInfoList* tList) {
    tList->startIteration();
    TermInfo* tEntry;
    while (tList->hasMore()) {
      tEntry = tList->nextEntry();
      if ( isTITLE(idx->term(tEntry->id())) ) return true;
    }
    return false;
  }
  
  int isPRONOUN(const char* check) {
    return !strcmp(check, PRONOUN);
  }
  
  struct compareSW {
    double lambda;
    compareSW(double l) { lambda = l; }
    bool operator()(const MMRPassage p1, const MMRPassage p2) const {
      return p1.computeMMR(lambda) > p2.computeMMR(lambda);
    }
  };
  
}; // MMRSumm

#endif
