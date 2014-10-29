/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
  Author: fff, dmf
  03/2003 -- pull larger fns out of StructQueryRep header file
*/

#include "StructQueryRep.hpp"

using namespace lemur::api;

lemur::retrieval::QnList::~QnList() {
  for(i = 0; i < qnList.size(); i++)
    delete qnList[i];
}

/// flatten children of a node into weighted prox entries.
void lemur::retrieval::QueryNode::transformPassageOps() {
  double childWeight = 1.0/entries;
  QnList *cl = ch, *newList = new QnList(), *kidList;
  QueryNode *child, *kid;

  if (!cl) return;
    
  cl->startIteration();
  while (cl->hasMore()) {
    child = cl->popNode();
    if (child->isProxNode()) {
      child->setWeight(childWeight * child->weight());
      newList->push_back(child);
    } else {
      /// explode the kiddies into a flat list of weighted prox nodes.
      child->transformPassageOps();
      kidList = child->ch;
      /// distribute element weight across kiddies
      kidList->startIteration();
      while (kidList->hasMore()) {
        kid = kidList->popNode();
        kid->setWeight(childWeight * kid->weight());
        newList->push_back(kid);
      }
      delete(kidList);
      child->ch = NULL;
      delete(child);
    }
  }
  delete(cl);
  ch = newList;
  entries = newList->size();
}

/// weighted sum of prox children
/// all belief operators have already been removed/flattened.
double lemur::retrieval::PassageQNode::passageScore(const StructQryDocRep *dRep) const{
  const QueryNode *child;
  DOCID_T did = dRep->did;
  double score = 0;
  int tf;
  double totalWeight = 0, weight, idf;
    
  ch->startIteration();
  while(ch->hasMore()) {
    // all children are prox children.
    child = ch->nextNode();
    idf = dRep->computeIdfScore(child->dCnt);
    weight = child->weight(); 
    totalWeight += weight;
    tf = 0;
    if (child->nextProxItem(did)) {
      // compute proximity score for a passage
      int numPos = child->proxList->count();
      for(int k = 0; k < numPos; k++) {
        LOC_T pos = child->proxList->position(k);
        if (pos >= dRep->start && pos < dRep->end)
          tf++;
      }
    }
    score += weight * dRep->beliefScore(tf, idf);
  }
  if (totalWeight > 0) // normalized by the sum of the weights
    score /= totalWeight;
  return score;
}

// every query node create a docId-list for its parent, and will
// be deleted by its parent after the parent creates its own, so
// that there is only one docIdList for the root as the query
// net done completely.

void lemur::retrieval::TermQnode::copyDocList(int listlen, int tf,
                                              const DocInfoList *dl, int numDocs) {
  COUNT_T dc = numDocs + 1;
  COUNT_T nd = dc;
  int cnt = 0;
  bool *didList= new bool[dc];
  for(COUNT_T i = 0; i < dc; i++) didList[i] = false;
  if (listlen == 0) {
    dList = didList;
    nextDoc = nd;
    dCnt = 0;
    proxList = new ProxInfo();
    return;
  }
  proxList = new ProxInfo(listlen, tf, dl);
  startProxIteration();
  while (hasMoreProx()) {
    nextProxItem();
    dc = proxList->id();
    if(dc < nd)
      nd = dc;
    didList[dc] = true;
    cnt++;
  }
  dList = didList;
  nextDoc = nd;
  dCnt = cnt;
  startProxIteration(); // reset for eval
  if (hasMoreProx()) nextProxItem(); // set pointer onto first entry
}

void lemur::retrieval::QueryNode::unionDocList(int numDocs) {
  // Consume the children's dLists
  COUNT_T dc = numDocs + 1;
  COUNT_T nd = dc;
  int cnt = 0;
  ch->startIteration();
  // If all children are empty (such as all terms that are OOV)
  // initialize an empty doc list.
  if (! ch->hasMore()) {
    dList = new bool[dc];
    for(COUNT_T i = 0; i < dc; i++) dList[i] = false;
    nextDoc = nd; /// first elt
    dCnt = cnt;
    proxList = NULL;
    return;
  }
  QueryNode *child = ch->nextNode();
  bool *didList = child->dList;
  nd = child->nextDoc; /// first elt
  cnt = child->dCnt;
  child->dList = NULL;
  while (ch->hasMore()) {
    child = ch->nextNode();
    int i;
    child->startProxIteration();
    if (child->hasMoreProx()) {
      while (child->hasMoreProx()) {
        child->nextProxItem();
        i = child->proxList->id();
        if(!didList[i]) {
          didList[i] = true;
          cnt++;
          if(i < nd)
            nd = i;
        }
      }
      child->startProxIteration(); // reset for eval
      if (child->hasMoreProx()) 
        child->nextProxItem(); // set pointer onto first entry
    } else {
      for(i = 1; i < dc; i++) {
        if(!didList[i] && child->dList[i]) {
          didList[i] = child->dList[i];
          cnt++;
          if(i < nd) // find a min doc-id for nextDoc
            nd = i;
        }
      }
    }
    delete[](child->dList);
    child->dList = NULL;
  }
  dList = didList;
  nextDoc = nd; /// first elt
  dCnt = cnt;
  proxList = NULL;
}

void lemur::retrieval::QueryNode::intersectDocList(int numDocs) {
  // rewrite this to be cleaner/neater/quicker...
  // should not have to do the allocs.
  int all = entries;
  COUNT_T dc = numDocs + 1;
  COUNT_T nd = dc;
  int cnt = 0;
  bool *didList = new bool[dc];
  COUNT_T *counts = new COUNT_T[dc];
  for(COUNT_T i=0; i<dc; i++) {didList[i]=false; counts[i]=0; };
  ch->startIteration();
  while (ch->hasMore()) {
    COUNT_T i;
    const QueryNode *child = ch->nextNode();
    child->startProxIteration();
    /// this should iterate over the sets of prox lists
    /// pairwise intersect (or clever).
    if (child->hasMoreProx()) {
      while (child->hasMoreProx()) {
        child->nextProxItem();
        i = child->proxList->id();
        counts[i]++;
        if(counts[i] == all) {
          didList[i] = true;
          cnt++;
          if(i < nd)
            nd=i;         
        }
      }
      child->startProxIteration(); // reset for eval
      if (child->hasMoreProx()) 
        child->nextProxItem(); // set pointer onto first entry
    } else {
      for(i = 1; i < dc; i++)
        if(child->dList[i]) {
          counts[i]++;
          if(counts[i] == all) {
            didList[i] = true;
            cnt++;
            if(i < nd)
              nd = i;
          }
        }
    }
  }
  dList = didList;
  nextDoc = nd;
  dCnt = cnt;
  proxList = NULL;
  delete[](counts);
}

void lemur::retrieval::SynQNode::synonymProxList() {
  // Called after child doclists have been unioned.
  // Have to union the proxinfos.
  // dCnt, dList, nextDoc all have been set.
  // proxList is NULL.
  const QueryNode *child;
  int numChildren = entries;  
  int nextPos, minIDX, minVal;
  int i, j, k,  df = 0;
  /// we know how many entries are coming.
  /// total requirement is sum of the sizes of each child list + 2*dCnt
  int totalPos = 2 * dCnt, totalTF = 0;
  for (k = 0; k < numChildren; k++) {
    child = ch->getNode(k);
    child->startProxIteration(); // set up for iteration
    totalTF += child->proxList->posSize();    // count position entries
  }
  totalPos += totalTF;
  LOC_T *newProx = new LOC_T[totalPos];
  COUNT_T *tmpDF = new COUNT_T[numChildren]; /// collect freqs
  const LOC_T** tmpPos = new const LOC_T *[numChildren]; /// collect pos vectors to merge.
  LOC_T *posIDX = new LOC_T[numChildren]; /// pos buffer idx
  int newProxIDX = 0;
  for(j = 0, i = nextDoc; j < dCnt; i++) {

    if(dList[i]) {
      /// initialize
      // get prox info for all children
      j++;
      df = 0;
      for (int k = 0; k < numChildren; k++) {
        child = ch->getNode(k);
        posIDX[k] = 0;
        if (child->nextProxItem(i)){
          tmpDF[k] = child->proxList->count();
          df += tmpDF[k];
          tmpPos[k] = child->proxList->positions();
        } else {
          tmpDF[k] = 0;
          tmpPos[k] = NULL;
        }
      }
      newProx[newProxIDX++] = i; // docid
      newProx[newProxIDX++] = df; // df (total number of positions)
      /// merge arrays into posList
      for (nextPos = 0; nextPos < df; nextPos++) {
        minIDX = 0;
        minVal = INT_MAX;
        // choose next minimum position entry.
        for(int k = 0; k < numChildren; k++) {
          if (posIDX[k] < tmpDF[k])
            if (tmpPos[k][posIDX[k]] < minVal) {
              minVal = tmpPos[k][posIDX[k]];
              minIDX = k;
            }
        }
        newProx[newProxIDX++] = tmpPos[minIDX][posIDX[minIDX]]; // next pos
        posIDX[minIDX]++; // advance the indicator
      }
    }
  }
  // delete all children
  delete ch;
  delete[](tmpDF);
  delete[](tmpPos);
  delete[](posIDX);
  proxList = new ProxInfo(dCnt, totalTF, newProx);
  proxList->size = numChildren;  /// flag for evaluator.
  startProxIteration(); // reset for eval pass
  if (hasMoreProx()) nextProxItem(); // set pointer to first entry.  
  ch = NULL;
}


bool lemur::retrieval::OdnQNode::foundOrderedProx(int currPos, int wsize, 
                                                  const QnList *cl,  int ith) {
  // recursively find the matched window in children order
  if(ith < cl->size()) {
    const QueryNode *nextChild = cl->getNode(ith);
    if(nextChild->proxList->nextPos >= nextChild->proxList->count())
      return false; // no more positions for this child
    int childPos;
    childPos = nextChild->proxList->position(nextChild->proxList->nextPos);
    if (childPos < currPos) {
      while (nextChild->proxList->nextPos < nextChild->proxList->count() &&
             ((childPos = nextChild->proxList->position(nextChild->proxList->nextPos)) < currPos)) {
        nextChild->proxList->nextPos++;
      }
      if(nextChild->proxList->nextPos >= nextChild->proxList->count())
        return false; // no more positions for this child
    }
    if((childPos - currPos - 1) < wsize)
      // matching the next child
      return foundOrderedProx(childPos, wsize, cl, ith + 1);
    else
      return false;
  } else {
    // matched all children recursively
    return true;
  }
}

void lemur::retrieval::OdnQNode::orderedProxList(int numDocs) {
  static int intsize = sizeof(int);
  const QueryNode *child;
  int i, j, cnt, df = 0;
  COUNT_T nd = numDocs + 1;

  // Choose min num doc positions for allocation.
  int totalPos = INT_MAX;
  ch->startIteration();
  while (ch->hasMore()) {
    child = ch->nextNode();
    if (child->proxList->posSize() < totalPos) {
      totalPos = child->proxList->posSize();      
    }
  }
  totalPos += dCnt * 2;
  LOC_T *tmpPos = NULL;
  LOC_T *newProx = new LOC_T[totalPos];
  int newProxIDX = 0;
  int totalTF = 0;
  
  for(j = 0, i = nextDoc; j < dCnt; i++) {
    if(dList[i]) {  /// doc in the list
      j++;
      /// ensure everyone is on the same doc id
      ch->startIteration();
      while (ch->hasMore()) {
        child = ch->nextNode();
        child->nextProxItem(i);
        // initialize the prox with the first occurring position 
        child->proxList->nextPos = 0;
      }
      cnt = 0;
      // compute the prox info for current doc
      // start with the first child
      child = ch->getNode(0);
      int numPos = child->proxList->count();
      const LOC_T *positions = child->proxList->positions();
      for(int k = 0; k < numPos; k++) {
        // find the matched window from the rest of children
        if(foundOrderedProx(positions[k], winSize, ch, 1)) {
          if (cnt == 0) { // first one.
            delete[](tmpPos);
            /// maximum number of matches.
            tmpPos = new LOC_T[numPos];
          }
          tmpPos[cnt++]= positions[k];
          // Advance each child to the next occurring position
          // consume the used position entries
          ch->startIteration();
          while (ch->hasMore()) {
            child = ch->nextNode();
            child->proxList->nextPos++;
          }
        }
      }
      if(cnt > 0) {
        newProx[newProxIDX++] = i; // docid
        newProx[newProxIDX++] = cnt; // (total number of matched positions)
        memcpy((void *)(newProx + newProxIDX), (void *) tmpPos, 
               cnt * sizeof(LOC_T));
        totalTF += cnt;
        newProxIDX += cnt;
        df++;
        if(i < nd) nd = i;
      } else {
        dList[i]=false;
      }
    }
  }
  dCnt = df;
  if (df) {
    proxList = new ProxInfo(df, totalTF, newProx); 
  } else {
    proxList = new ProxInfo(); 
    delete[](newProx);
  }
  proxList->size = winSize;
  startProxIteration(); // reset for eval
  if (hasMoreProx()) nextProxItem(); // set pointer to first entry.
  nextDoc = nd;
  // delete all children
  delete ch;
  delete[](tmpPos);
  ch = NULL;
}

bool lemur::retrieval::UwnQNode::findUnorderedWin(const QueryNode *cqn, QnList *cl, 
                                                  int winSize) {
  bool found = true;
  cl->startIteration();
  while (found && cl->hasMore()) {
    QueryNode *child = cl->nextNode();
    if(child == cqn)
      continue;
    found = (child->proxList->position(child->proxList->nextPos) <= winSize);
  }
  return found;
}



void lemur::retrieval::UwnQNode::unorderedProxList(int numDocs) {
  // should special case when there is but a single child.
  static int intsize = sizeof(int);
  QueryNode *child; 
  int i, j, cnt, df = 0;
  COUNT_T nd = numDocs + 1;
  //  int winSize = ((UwnQNode *)qn)->winSize;

  // Choose min num doc positions for allocation.
  int totalPos = INT_MAX;
  ch->startIteration();
  while (ch->hasMore()) {
    child = ch->nextNode();
    if (child->proxList->posSize() < totalPos)
      totalPos = child->proxList->posSize();
  }

  totalPos += 2 * dCnt;
  LOC_T *newProx = new LOC_T[totalPos];
  LOC_T *tmpPos = NULL;
  int newProxIDX = 0;
  int totalTF = 0;
  for (j = 0, i = nextDoc; j < dCnt; i++) {
    if(dList[i]) {
      j++;
      /// ensure everyone is on the same doc id
      ch->startIteration();
      while (ch->hasMore()) {
        child = ch->nextNode();
        child->nextProxItem(i);
        // initialize the prox with the first occurring position 
        child->proxList->nextPos = 0;
      }
      // compute the prox info for current doc
      cnt = 0;
      // start with the child having the minimum occurring position
      int startPos = INT_MAX;
      QueryNode *startChild = NULL;
      while (startPos > 0) {
        // find the minimun occurring position
        ch->startIteration();
        while (ch->hasMore()) {
          child = ch->nextNode();
          LOC_T pos = child->proxList->position(child->proxList->nextPos);
          if(pos < startPos) {
            startPos = pos;
            startChild = child;
          }
        }
        // find the matched window from the rest of children
        if(findUnorderedWin(startChild, ch, startPos + winSize)) {
          if (cnt == 0) { // first one.
            delete[](tmpPos);
            /// maximum number of matches.
            tmpPos = new LOC_T[startChild->proxList->count()];
          }
          tmpPos[cnt++]= startPos;
          // consume the used position entries
          ch->startIteration();
          // move the startPos to the next occurring position
          startPos = INT_MAX;
          while (ch->hasMore()) {
            child = ch->nextNode();
            child->proxList->nextPos++;
            if(child->proxList->nextPos >= child->proxList->count())
              startPos = 0; // no more of this child. break loop.
          }
        } else {
          // move the startPos to the next occurring position
          // for the failed node
          startChild->proxList->nextPos++;
          if(startChild->proxList->nextPos < startChild->proxList->count())
            startPos = INT_MAX;
          else
            startPos = 0;  // no more of this child. break loop.
        }
      }
      
      if(cnt > 0) {
        newProx[newProxIDX++] = i; // docid
        newProx[newProxIDX++] = cnt; // (total number of matched positions)
        memcpy((void *)(newProx + newProxIDX), (void *) tmpPos, 
               cnt * sizeof(LOC_T));
        totalTF += cnt;
        newProxIDX += cnt;
        df++;
        if(i < nd) nd = i;
      } else {
        dList[i] = false;
      }
    }
  }
  dCnt = df;
  if (df) {
    proxList = new ProxInfo(df, totalTF, newProx); 
  } else {
    proxList = new ProxInfo(); 
    delete[](newProx);
  }
  proxList->size = winSize;
  startProxIteration(); // reset for eval
  if (hasMoreProx()) nextProxItem(); // set pointer to first entry.
  nextDoc = nd;
  // delete all children
  delete ch;
  delete[](tmpPos);
  ch = NULL;
}
