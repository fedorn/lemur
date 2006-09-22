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

#include "MMRSumm.hpp"

using namespace lemur::api;

const string lemur::summarization::MMRSumm::TITLE = "*title";
const string lemur::summarization::MMRSumm::PRONOUN = "*pronoun";

void lemur::summarization::MMRSumm::markPassages(int optLen, 
                                                 const string &qInfo) {
  int oldLen = -1;
  if (optLen != -1) {
    oldLen = summLen;
    summLen = optLen;
  }
  int ctr = 1;
  vector<MMRPassage> copyPsgs(doc);
  vector<MMRPassage>::iterator it;
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
  
void lemur::summarization::MMRSumm::addPassage(Passage &psg) {
  MMRPassage* mPsg = (MMRPassage*)(&psg);
  doc.push_back(*mPsg);
}  

void lemur::summarization::MMRSumm::addDocument(const string &docID) {

  int eos = 0;

  MMRPassage* tempPsg;

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
    tempPsg = new MMRPassage(docID);
    findNextPassage(*tempPsg, idx, tList, eos);
    addPassage(*tempPsg);
    // the vector makes a copy, delete this.
    delete(tempPsg);
  }
  delete tList;

}

  
int lemur::summarization::MMRSumm::fetchPassages(Passage* psgs, 
                                                 int optLen) const {
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
  
void lemur::summarization::MMRSumm::findNextPassage(MMRPassage &psg, 
                                                    const lemur::api::Index* idx, 
                                                    const TermInfoList* tList, int eos) {
  TermInfo* tEntry;
  psg.clear();
  // The vector makes a copy, this leaks.
  //  termCount* storage;
  termCount storage;
  if (eos) {
    while (tList->hasMore()) {
      tEntry = tList->nextEntry();
      if ( isEOS(idx->term(tEntry->termID())) ) return;
      //      storage = new termCount;
      //      storage->termID = tEntry->termID();
      //      storage->tf = tEntry->count();
      //      storage->val = tEntry->count();
      //      psg.addTerm(*storage);
      storage.termID = tEntry->termID();
      storage.tf = tEntry->count();
      storage.val = tEntry->count();
      psg.addTerm(storage);
    }
  } else {
    for(int i=0; i < PSG_LEN; i++) {
      if (tList->hasMore()) {
        tEntry = tList->nextEntry();
        //        storage = new termCount;
        //        storage->termID = tEntry->termID();
        //        storage->tf = tEntry->count();
        //        storage->val = tEntry->count();
        //        psg.addTerm(*storage);
        storage.termID = tEntry->termID();
        storage.tf = tEntry->count();
        storage.val = tEntry->count();
        psg.addTerm(storage);
      } else {
        return;
      }
    }
  }
  return;
}
  
void lemur::summarization::MMRSumm::showPassage(const passageVec* psg, 
                                                const lemur::api::Index* idx) const {
  for (int i=0; i < psg->size(); i++) {
    cout << idx->term((*psg)[i].termID) << " ";
  }
}

void lemur::summarization::MMRSumm::showMarkedPassages() const {
  for (int i=0; i<doc.size(); i++) {
    if (doc[i].marked > 0) {
      showPassage(doc[i].getAsVector(), idx);
      cout << endl;
    }
  }
}

void lemur::summarization::MMRSumm::summDocument(const string &docID, 
                                                 const int optLen, 
                                                 const string &qInfo) {
  int oldLen = -1;
  queryPassage = new MMRPassage(docID);
  if (optLen != -1) {
    oldLen = summLen;
    summLen = optLen;
  }
  int eos = 0;
  
  if (qInfo.empty()) {
    autoMMRQuery();
  } else {
    setMMRQuery(qInfo);
  }
  
  MMRPassage* tempPsg;
  
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
    tempPsg = new MMRPassage(docID);
    findNextPassage(*tempPsg, idx, tList, eos);
    addPassage(*tempPsg);
    delete(tempPsg);
  }
  delete tList;
  
  scorePassages(qInfo);
  markPassages(-1, "");
  
  if (oldLen != -1) {
    summLen = oldLen;
  }
  
}  

void lemur::summarization::MMRSumm::scorePassages(const string &qInfo) {
  // query similarities
  int saveScore = doc.size();
  vector<MMRPassage> docCopy(doc);
  for(int iT=0; iT<doc.size(); iT++) {
    docCopy[iT].qSim = docCopy[iT].dotProduct(*queryPassage);
    docCopy[iT].wtSim = docCopy[iT].qSim * docCopy[iT].wt;
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
        double dP = (*j).dotProduct(*k);
        if (dP > (*j).maxSim) {
          (*j).maxSim = dP;
        }
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

void lemur::summarization::MMRSumm::clear(void) {
  doc.erase(doc.begin(), doc.end());
  iterCount = 1;
}

int lemur::summarization::MMRSumm::nextPassage(Passage* psg) const {
  for (int i=0; i<doc.size(); i++) {
    if (doc[i].marked  == iterCount) {
      *psg = doc[i];
      iterCount++;
      return i;
    }
  }
  return 0;
}

void lemur::summarization::MMRSumm::iterClear(void) const {
  iterCount = 1;
}

void lemur::summarization::MMRSumm::outputSumm(void) const {
  showMarkedPassages();
}




