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

#include "MMRSumm.hpp"

void MMRSumm::markPassages(int optLen = -1, char* qInfo = NULL) {
  int oldLen;
  if (optLen != -1) {
    oldLen = summLen;
    summLen = optLen;
  }
  int ctr = 1;
  //scorePassages(qInfo);
  vector<MMRPassage> copyPsgs(doc);
  vector<MMRPassage>::iterator it;
  for (int i=0; i< summLen; i++) {
    it = max_element(copyPsgs.begin(), copyPsgs.end());
    (*it).score = -1;
    (*it).marked = ctr++;
  }
  for (int i=0; i< doc.size(); i++) {
    doc[i].marked = copyPsgs[i].marked;
  }
  
  if (oldLen != -1) {
    summLen = oldLen;
  }    
}
  
void MMRSumm::addPassage(Passage &psg) {
  MMRPassage* mPsg = dynamic_cast<MMRPassage*>(&psg);
  doc.push_back(*mPsg);
}  

  
int MMRSumm::fetchPassages(Passage psgs[], int optLen = -1) {
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
  
void MMRSumm::findNextPassage(MMRPassage &psg, InvFPIndex* idx, 
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
      storage->val = tEntry->count();
      psg.addTerm(*storage);
    }
  } else {
    for(int i=0; i < PSG_LEN; i++) {
      if (tList->hasMore()) {
	tEntry = tList->nextEntry();
	storage = new termCount;
	storage->termID = tEntry->id();
	storage->tf = tEntry->count();
	storage->val = tEntry->count();
	psg.addTerm(*storage);
      } else {
	return;
      }
    }
  }
  return;
}
  
void MMRSumm::showPassage(passageVec* psg, InvFPIndex* idx) {
  for (int i=0; i < psg->size(); i++) {
    cout << idx->term((*psg)[i].termID) << " ";
  }
}

void MMRSumm::showMarkedPassages() {
  for (int i=0; i<doc.size(); i++) {
    if (doc[i].marked > 0) {
      showPassage(doc[i].getAsVector(), idx);
      cout << endl;
    }
  }
}

void MMRSumm::summDocument(const char* docID, int optLen = -1, const char* qInfo = NULL) {
  int oldLen;
  queryPassage = new MMRPassage(docID);
  if (optLen != -1) {
    oldLen = summLen;
    summLen = optLen;
  }
  int eos = 0;
  
  if (!qInfo) {
    autoMMRQuery();
  } else {
    setMMRQuery((char*)qInfo);
  }
  
  MMRPassage* tempPsg;
  
  TermInfoList* tList = idx->termInfoListSeq(idx->document(docID));
  eos = hasEOS(idx, tList);
  if (eos) {
    cout << "EOS located in document" << endl;
  } else {
    cout << "EOS not present; using default passage length " << summLen << endl;
  }
  tList->startIteration();
  while (tList->hasMore()) {
    tempPsg = new MMRPassage(docID);
    findNextPassage(*tempPsg, idx, tList, eos);
    addPassage(*tempPsg);
  }
  delete tList;
  
  scorePassages(qInfo);
  markPassages();
  
  if (oldLen != -1) {
    summLen = oldLen;
  }
  
}  

void MMRSumm::scorePassages(const char* qInfo = NULL) {
  // query similarities
  int saveScore = doc.size();
  vector<MMRPassage> docCopy(doc);
  for(int i=0; i<doc.size(); i++) {
    docCopy[i].qSim = docCopy[i].dotProduct(*queryPassage);
    docCopy[i].wtSim = docCopy[i].qSim * docCopy[i].wt;
  }
  
  sort(docCopy.begin(), docCopy.end(), compareSW(lambda));
  docCopy[0].score = saveScore;
  saveScore--;
  vector<MMRPassage>::iterator i, j, k;
  i = docCopy.begin();
  while ((i < docCopy.end()) && ((*i).wtSim > 0.0)) {
    double maxMMR = -1.0;
    j=i;
    j++;
    while ((j < docCopy.end()) && ((*j).computeMMR(lambda) > maxMMR)) {
      for(k = docCopy.begin(); k<i; k++) {
	(*j).maxSim = (*j).dotProduct(*k);
      }
      double s=(*j).computeMMR(lambda);
      if (s>maxMMR) {
	maxMMR = s;
      }
      j++;
    }
    (*i).score = saveScore;
    saveScore--;
    sort(++i, j, compareSW(lambda));
  }
  doc = docCopy;
  
}

void MMRSumm::clear(void) {
  doc.erase(doc.begin(), doc.end());
  iterCount = 1;
}

int MMRSumm::nextPassage(Passage* psg) {
  for (int i=0; i<doc.size(); i++) {
    if (doc[i].marked  == iterCount) {
      *psg = doc[i];
      iterCount++;
      return i;
    }
  }
  return 0;
}

void MMRSumm::iterClear(void) {
  iterCount = 1;
}

void MMRSumm::outputSumm(void) {
  showMarkedPassages();
}




