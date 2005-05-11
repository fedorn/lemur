/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "BasicSumm.hpp"

using namespace lemur::api;

void lemur::summarization::BasicSumm::markPassages(int optLen, 
                                                   const string &qInfo) {
  int oldLen = -1;
  if (optLen != -1) {
    oldLen = summLen;
    summLen = optLen;
  }
  int ctr = 1;
  scorePassages(qInfo);
  vector<BasicPassage> copyPsgs(doc);
  vector<BasicPassage>::iterator it;
  for (int i=0; i< summLen; i++) {
    it = max_element(copyPsgs.begin(), copyPsgs.end());
    (*it).score = -1;
    (*it).marked = ctr++;
  }
  for (int j=0; j< doc.size(); j++) {
    doc[j].marked = copyPsgs[j].marked;
  }
  
  if (oldLen != -1) {
    summLen = oldLen;
  }    
}

void lemur::summarization::BasicSumm::addPassage(Passage &psg) {
  BasicPassage* bPsg = dynamic_cast<BasicPassage*>(&psg);
  doc.push_back(*bPsg);
}  

int lemur::summarization::BasicSumm::fetchPassages(Passage* psgs, 
                                                   int optLen = -1) const {
  int l, count=0;
  if (optLen >0) {
    l = optLen;
  } else {
    l = summLen;
  }
  if (l > doc.size()) l = doc.size();
  for (int i=0; i<doc.size() ; i++) {
    if (doc[i].marked) {
      psgs[count] = doc[i];
      count++;
    }
  }
  return count;
}
  
void lemur::summarization::BasicSumm::summDocument(const string &docID, 
                                                   const int optLen, 
                                                   const string &qInfo) {
  int oldLen = -1;
  if (optLen != -1) {
    oldLen = summLen;
    summLen = optLen;
  }
  int eos = 0;
  
  BasicPassage* tempPsg;
  
  TermInfoList* tList = idx->termInfoListSeq(idx->document(docID));
  eos = hasEOS(idx, tList);
  if (eos) {
    cout << "EOS located in document" << endl;
  } else {
    cout << "EOS not present; using default passage length " 
         << summLen << endl;
  }
  tList->startIteration();
  while (tList->hasMore()) {
    tempPsg = new BasicPassage(docID);
    findNextPassage(*tempPsg, idx, tList, eos);
    addPassage(*tempPsg);
    // the vector makes a copy, delete this
    delete(tempPsg);
  }
  delete tList;
  
  scorePassages(qInfo);
  markPassages(-1, "");
  
  if (oldLen != -1) {
    summLen = oldLen;
  }
  
} 

void lemur::summarization::BasicSumm::scorePassages(const string &qInfo) {
  for (int i=0; i < doc.size(); i++) {
    scorePassage(doc[i], qInfo);
  }    
}

void lemur::summarization::BasicSumm::clear(void) {
  doc.erase(doc.begin(), doc.end());
  iterCount = 1;
}

int lemur::summarization::BasicSumm::nextPassage(Passage* psg) const {
  for (int i=0; i<doc.size(); i++) {
    if (doc[i].marked  == iterCount) {
      *psg = doc[i];
      iterCount++;
      return i;
    }
  }
  return 0;
}

void lemur::summarization::BasicSumm::iterClear(void) const {
  iterCount = 1;
}

void lemur::summarization::BasicSumm::outputSumm(void) const {
  showMarkedPassages();
}

