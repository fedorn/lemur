/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  Author: fff
 */

#include "StructQueryRep.hpp"

QnList::~QnList() {
  for(i=0; i<qnList.size(); i++)
    delete qnList[i];
  
}

void StructQueryRep::termOffsetList(QueryNode *qn, int did) {
  ProxInfo *proxList=NULL;
  ProxInfo *curr;
  int i = 0;
  int qid=qn->id();

  curr = new ProxInfo;
  curr->id = did;
  // assume no more than  5*docLengthAvg same terms in a doc 
  //  curr->posList = new int[(int)ind.docLengthAvg()*5];
  curr->posList = new int[(int)ind.docLength(did)];
  TermInfoList *dTerms=ind.termInfoListSeq(did);
  InvFPTerm *thisTerm;
  dTerms->startIteration();
  while (dTerms->hasMore()) {
    // this loop should be used to create a proxlist for all children
    // because all children of a prox op must occur in this doc.
    thisTerm = (InvFPTerm *) dTerms->nextEntry();
    if(qid == thisTerm->id()) {
      curr->posList[i++] = thisTerm->position();
      curr->count++;
    }
  }
  delete dTerms;
  curr->size=0;
  curr->next=NULL;
  if(!qn->proxList) {
    qn->proxList=curr;
    qn->nextDoc=did;
  } else {
    // put the current one at the end of the list
    proxList=qn->proxList;
    while(proxList->next)
      proxList=proxList->next;
    proxList->next=curr;
  }
}


void StructQueryRep::synonymProxList(QueryNode *qn) {
  ProxInfo *childProx;
  ProxInfo *curr;
  ProxInfo *prxl=NULL;
  qn->proxList=NULL;
  QnList *cl = qn->ch;
  QueryNode *child;
  int i, j, k, cnt,  df=0;
  int nd = ind.docCount()+1;

  for(j=0, i = qn->nextDoc; j<qn->dCnt; i++) {
    if(qn->dList[i]) {
      // int dl=ind.docLength(i)+1; can't use docLength because doc maybe stopped
      int dl=ind.docLength(i)*10; // large enough
      bool *posBuff = new bool[dl];
      for(k=0; k<dl; k++) posBuff[k]=false;
      cnt=0;
      j++;
      // get prox info for all children
      cl->startIteration();
      while (cl->hasMore()) {
	child = cl->nextNode();
	if(child->proxList) {
	  // if doc id of child prox is less than the current
	  // one (i), remove the child prox until its doc id > i
	  childProx =child->proxList;
	  while(childProx && childProx->id<i) {
	    childProx = childProx->next;
	    delete child->proxList;
	    child->proxList= childProx;
	  }
	  if(!child->proxList) {
	    // should not be happend if dCnt>0
	    qn->nextDoc = ind.docCount();
	    qn->dCnt=0;
	    return;
	  }
	} else {
	  // if the proxList is still NULL, the child must be a term 
	  // rather then other prox ops
	  termOffsetList(child, i);
	}
	childProx=child->proxList;
	// record every occ, merging children occurrences 
	for(k=0; k< childProx->count; k++)
	  if(childProx->posList[k]<dl && !posBuff[childProx->posList[k]]) {
	    // for every position count only once
	    posBuff[childProx->posList[k]]=true;
	    cnt++;
	  }
      }
      if(cnt>0) {
	// merging children proxList
	curr=new ProxInfo;
	curr->size=((OdnQNode *)qn)->winSize;
	curr->id=i;
	curr->posList= new int[cnt];
	cnt=0;
	// copy the occurrence position from posBuff
	for(k=0; k< dl; k++)
	  if(posBuff[k])
	    curr->posList[cnt++]=k;
	if(cnt>0) {
	  curr->count=cnt;
	  if(!qn->proxList) {
	    qn->proxList=curr;
	    prxl=curr;
	  } else {
	    prxl->next=curr;
	    prxl=curr;
	  }
	  df++;
	  if(i < nd) nd=i;
	}
	else {
	  qn->dList[i]=false;
	}
      }
      delete []posBuff;
      // delete proxList for curr doc for all children
      cl->startIteration();
      while (cl->hasMore()) {
	child = cl->nextNode();
	if(child->proxList) {
	  childProx =child->proxList->next;
	  delete child->proxList;
	  child->proxList= childProx;
	}
      }
    }
  }
  qn->dCnt=df;
  qn->nextDoc=nd;
  // delete all children, should have a destructor to do so
  delete cl;
  qn->ch=NULL;
}


bool StructQueryRep::foundOrderedProx(int currPos, int wsize, QnList *cl, int ith) {
  // recursively find the matched window in children order
  if(ith < cl->size()) {
    QueryNode *nextChild = cl->getNode(ith);
    ProxInfo *nextPrx = nextChild->proxList;
    for(int k=0; k< nextPrx->count; k++) {
      if(nextPrx->posList[k] < currPos)
	continue;
      if(nextPrx->posList[k]-currPos-1 < wsize)
	// matching the next child
	return foundOrderedProx(nextPrx->posList[k], wsize, cl, ith+1);
      else
	return false;
    }
    return false;
  }
  else
    // matched all children recursively
    return true;
}

void StructQueryRep::orderedProxList(QueryNode *qn) {
  ProxInfo *childProx;
  ProxInfo *curr;
  ProxInfo *prxl=NULL;
  qn->proxList=NULL;
  QnList *cl = qn->ch;
  QueryNode *child;
  int i, j, cnt, df=0;
  int nd = ind.docCount()+1;
  bool no_more=false;

  for(j=0, i = qn->nextDoc; j<qn->dCnt; i++) {
    if(qn->dList[i]) {
      j++;
      // get prox info for all children
      cl->startIteration();
      while (cl->hasMore()) {
	child = cl->nextNode();
	if(child->proxList) {
	  // if doc id of child prox is less than the current
	  // one (i), remove the child prox until its doc id > i
	  childProx =child->proxList;
	  while(childProx && childProx->id<i) {
	    childProx = childProx->next;
	    delete child->proxList;
	    child->proxList= childProx;
	  }
	  if(!child->proxList) {
	    // should not be happend if dCnt>0
	    qn->nextDoc = ind.docCount();
	    qn->dCnt=0;
	    return;
	  }
	} else {
	  // if the proxList is still NULL, the child must be a term 
	  // rather then other prox ops
	  termOffsetList(child, i);
	}
      }
      curr=NULL;
      cnt=0;
      // compute the prox info for current doc
      // start with the first child
      child = cl->getNode(0);
      childProx = child->proxList;
      for(int k=0; k<childProx->count;k++) {
	// find the matched window from the rest of children
	if(foundOrderedProx(childProx->posList[k], ((OdnQNode *)qn)->winSize, cl, 1)) {
	  if(!curr) {
	    curr= new ProxInfo;
	    curr->size=((OdnQNode *)qn)->winSize;
	    curr->id=i;
	    curr->posList= new int[childProx->count];
	  }
	  curr->posList[cnt++]= childProx->posList[k];
	}
      }
      if(cnt>0) {
	curr->count=cnt;
	if(!qn->proxList) {
	  qn->proxList=curr;
	  prxl=curr;
	} else {
	  prxl->next=curr;
	  prxl=curr;
	}
	df++;
	if(i < nd) nd=i;
      }
      else {
	qn->dList[i]=false;
      }
      // delete proxList for curr doc for all children
      cl->startIteration();
      while (cl->hasMore()) {
	child = cl->nextNode();
	if(child->proxList) {
	  if(child->proxList->size>0 && !child->proxList->next) {
	    // if child is a prox op (size>0) and no more docs, then done
	    no_more=true;
	  }
	  childProx =child->proxList->next;
	  delete child->proxList;
	  child->proxList= childProx;
	  if(no_more)
	    break;
	}
      }
    }
    if(no_more)
      break;
  }
  qn->dCnt=df;
  qn->nextDoc=nd;
  // delete all children, should have a destructor to do so
  delete cl;
  qn->ch=NULL;
}

bool StructQueryRep::findUnorderedWin(QueryNode *cqn, QnList *cl, int winSize) {
  bool found=true;
  ProxInfo *childProx;
  cl->startIteration();
  while (cl->hasMore()) {
    QueryNode *child = cl->nextNode();
    if(child == cqn)
      continue;
    childProx=child->proxList;
    if(childProx->posList[childProx->nextPos] > winSize) {
      // out of window
      found=false;
      break;
    }
  }
  return found;
}



void StructQueryRep::unorderedProxList(QueryNode *qn) {
  ProxInfo *childProx;
  ProxInfo *curr;
  ProxInfo *prxl=NULL;
  qn->proxList=NULL;
  QnList *cl = qn->ch;
  QueryNode *child; 
  int i, j, cnt, maxOcc=0, df=0;
  int nd = ind.docCount()+1;
  bool no_more=false;
  for(j=0, i = qn->nextDoc; j<qn->dCnt; i++) {
    if(qn->dList[i]) {
      j++;
      // get prox info for all children
      cl->startIteration();
      while (cl->hasMore()) {
	child = cl->nextNode();
	if(child->proxList) {
	  // if doc id of child prox is less than the current
	  // one (i), remove the child prox until its doc id > i
	  childProx =child->proxList;
	  while(childProx && childProx->id<i) {
	    childProx = childProx->next;
	    delete child->proxList;
	    child->proxList= childProx;
	  }
	  if(!child->proxList) {
	    // should not be happend if dCnt>0
	    qn->nextDoc = ind.docCount();
	    qn->dCnt=0;
	    return;
	  }
	} else {
	  // if the proxList is still NULL, the child must be a term 
	  // rather then other prox ops
	  termOffsetList(child, i);
	}
      }
      cnt=0;
      maxOcc = 0;
      curr=NULL;
      // compute the prox info for current doc
      // initialize the prox with the first occurring position 
      cl->startIteration();
      while (cl->hasMore()) {
	child = cl->nextNode();
	child->proxList->nextPos=0;
	if(child->proxList->count>maxOcc)
	  maxOcc=child->proxList->count;
      }
      // start with the child having the minimum occurring position
      // can not use the doclength, which maybe less than
      // a occurrence position if the database is stopped.
      // int startPos=ind.docLength(i);
      int startPos=INT_MAX;
      QueryNode *startChild=NULL;
      cnt =0;
      while (startPos > 0) {
	// find the minimun occurring position
	cl->startIteration();
	while (cl->hasMore()) {
	  child = cl->nextNode();
	  childProx=child->proxList;
	  if(childProx->posList[childProx->nextPos] < startPos) {
	    startPos=childProx->posList[childProx->nextPos];
	    startChild=child;
	  }
	}
	// cout << ind.document(i) << " " << startPos << endl;
	// find the matched window from the rest of children
	if(findUnorderedWin(startChild,cl,startPos+((UwnQNode *)qn)->winSize)) {
	  if(!curr) {
	    curr= new ProxInfo;
	    curr->size=((UwnQNode *)qn)->winSize;
	    curr->id=i;
	    // guarentee an enough room for matched positions
	    //	    curr->posList= new int[startChild->proxList->count+1];
	    // May have multiple matches backward/forward,
	    // so presume we need 10 * count of matched terms.
	    curr->posList= new int[maxOcc * 10];
	  }
	  if (cnt >= (maxOcc * 10))
	    cerr << "Too many posList entries: " << cnt << ":" 
		 << (maxOcc * 10) << ":" << ind.document(i) << endl;
	  curr->posList[cnt++]= startPos;
	}
	// move the startPos to the next occurring position
	childProx=startChild->proxList;
	childProx->nextPos++;
	if(childProx->nextPos < childProx->count)
	  startPos=INT_MAX;
	else
	  startPos=0;  // equal to break here
      }
      if(cnt>0) {
	curr->count=cnt;
	if(!qn->proxList) {
	  qn->proxList=curr;
	  prxl=curr;
	} else {
	  prxl->next=curr;
	  prxl=curr;
	}
	df++;
	if(i < nd) nd=i;
      } else {
	qn->dList[i]=false;
      }
      // delete proxList for curr doc for all children
      cl->startIteration();
      while (cl->hasMore()) {
	child = cl->nextNode();
	if(child->proxList) {
	  if(child->proxList->size>0 && !child->proxList->next) {
	    // if child is a prox op (size>0) and no more docs, then done
	    no_more=true;
	  }
	  childProx =child->proxList->next;
	  delete child->proxList;
	  child->proxList= childProx;
	  if(no_more)
	    break;
	}
      }
    }
    if(no_more)
      break;
  }
  qn->dCnt=df;
  qn->nextDoc=nd;
  // delete all children, should have a destructor to do so
  delete cl;
  qn->ch=NULL;
}



// every query node create a docId-list for its parent, and will
// be deleted by its parent after the parent creates its own, so
// that there is only one docIdList for the root as the query
// net done completely.

void StructQueryRep::copyDocList(DocInfoList *dl, QueryNode *qn) {
  int dc = ind.docCount()+1;
  int nd = dc;
  int cnt = 0;
  bool *didList= new bool[dc];
  for(int i=1; i<dc; i++) didList[i]=false;
  dl->startIteration();
  while (dl->hasMore()) {
    DocInfo *info = dl->nextEntry();
    dc = info->docID();
    if(dc < nd)
      nd=dc;
    didList[dc]=true;
    cnt++;
  }
  qn->dList=didList;
  qn->nextDoc=nd;
  qn->dCnt=cnt;
  qn->proxList=NULL;
}

void StructQueryRep::unionDocList(QueryNode *qn) {
  QnList *cl = qn->ch;
  int dc = ind.docCount()+1;
  int nd = dc;
  int cnt=0;
  bool *didList = new bool[dc];
  for(int i=1; i<dc; i++) didList[i]=false;
  cl->startIteration();
  while (cl->hasMore()) {
    int i;
    ProxInfo *prx;
    QueryNode *child = cl->nextNode();
    if((prx= child->proxList)) {
      while(prx) {
	i=prx->id;
	if(!didList[i]) {
	  didList[i]=true;
	  cnt++;
	  if(i < nd)
	    nd=i;
	}
	prx=prx->next;
      }
    }
    else {
      for(i=1; i < dc; i++) {
	if(!didList[i] && child->dList[i]) {
	  didList[i]=child->dList[i];
	  cnt++;
	  if(i < nd) // find a min doc-id for nextDoc
	    nd=i;
	}
      }
    }
  }
  qn->dList=didList;
  qn->nextDoc=nd;
  qn->dCnt=cnt;
  qn->proxList=NULL;
}

void StructQueryRep::intersectDocList(QueryNode *qn) {
  QnList *cl = qn->ch;
  int all = qn->entries;
  int dc = ind.docCount()+1;
  int nd = dc;
  int cnt=0;
  bool *didList = new bool[dc];
  int *counts = new int[dc];
  for(int i=1; i<dc; i++) {didList[i]=false; counts[i]=0; };
  cl->startIteration();
  while (cl->hasMore()) {
    int i;
    ProxInfo *prx;
    QueryNode *child = cl->nextNode();
    if((prx= child->proxList)) {
      while(prx) {
	i=prx->id;
	counts[i]++;
	if(counts[i] == all) {
	  didList[i]=true;
	  cnt++;
	  if(i < nd)
	    nd=i;
	}
	prx=prx->next;
      }
    }
    else {
      for(i=1; i < dc; i++)
	if(child->dList[i]) {
	  counts[i]++;
	  if(counts[i] == all) {
	    didList[i]=true;
	    cnt++;
	    if(i < nd)
	      nd=i;
	  }
	}
    }
  }
  qn->dList=didList;
  qn->nextDoc=nd;
  qn->dCnt=cnt;
  qn->proxList=NULL;
  delete []counts;
}

QnList * StructQueryRep::getChildren(StructQuery &qry) {
  TokenTerm *tok;
  QueryNode *qn;
  QueryNode *qParent= qStack[TopqStack-1];
  QnList *chlist= new QnList();
  int cnt=0;
  while(qry.hasMore()) {
    tok=qry.nextTerm();
    if(!strcmp(tok->spelling(), "RPAREN")) {
      if(qParent==qStack[--TopqStack]) {
	qParent->entries=cnt;
	return chlist;
      } else {
	cerr << "StructQueryRep::getChildren: Unmatched Parentheses" << endl;
	exit(1);
      }
    }
    else {
      qn=getQryNode(qry, tok, 1.0);
      if (qn != NULL) {
	chlist->push_back(qn);
	cnt++;
      }
    }
  }
  qParent->entries=cnt;
  return chlist;
}

QnList * StructQueryRep::getWeightedChildren(StructQuery &qry) {
  TokenTerm *tok;
  QueryNode *qn;
  QueryNode *qParent= qStack[TopqStack-1];
  QnList *chlist= new QnList();
  double wt;
  int cnt=0;
  while(qry.hasMore()) {
    tok=qry.nextTerm();
    if(!strcmp(tok->spelling(), "RPAREN")) {
      if(qParent==qStack[--TopqStack]) {
	qParent->entries=cnt;
	return chlist;
      } else {
	cerr << "StructQueryRep::getWeightedChildren: Unmatched Parentheses" << endl;
	exit(1);
      }
    }
    // each child has to have a weight and a qnode
    wt = (double)atof(tok->spelling());
    tok=qry.nextTerm();
    qn=getQryNode(qry, tok, wt);
    if (qn != NULL) {
      chlist->push_back(qn);
      cnt++;
    }
  }
  qParent->entries=cnt;
  return chlist;
}

QnList * StructQueryRep::getProxChildren(StructQuery &qry) {
  TokenTerm *tok;
  QueryNode *qn;
  QueryNode *qParent= qStack[TopqStack-1];
  QnList *chlist= new QnList();
  int cnt=0;
  while(qry.hasMore()) {
    tok=qry.nextTerm();
    if(!strcmp(tok->spelling(), "RPAREN")) {
      if(qParent==qStack[--TopqStack]) {
	qParent->entries=cnt;
	return chlist;
      } else {
	cerr << "StructQueryRep::getProxChildren: Unmatched Parentheses" << endl;
	exit(1);
      }
    }
    else {
      // only difference from getChildren
      qn=getProxQryNode(qry, tok);
      if (qn != NULL) {
	chlist->push_back(qn);
	cnt++;
      }
    }
  }
  qParent->entries=cnt;
  return chlist;
}

QueryNode * StructQueryRep::getProxQryNode(StructQuery &qry, TokenTerm *tok) {
  // using a separate function for parsing proximity ops
  // because they allow only proximity children
  QueryNode *qn;
  int ti = ind.term(tok->spelling());
  if (ti>0) {
    // found a indexed term
    qn = new TermQnode(ti, 1.0);
    DocInfoList *dl = ind.docInfoList(ti);
    copyDocList(dl, qn);
    delete dl;
    return qn;
  }
  // ODN
  else if(!strcmp(tok->spelling(),"#ODN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new OdnQNode(size, 1.0, dw);
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getProxChildren(qry);
    // get the list of doc's in which contain all query terms
    intersectDocList(qn);
    orderedProxList(qn);
    return qn;
  }
  // UWN
  else if(!strcmp(tok->spelling(),"#UWN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new UwnQNode(size, 1.0, dw);
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getProxChildren(qry);
    intersectDocList(qn);
    unorderedProxList(qn);
    return qn;
  }
  // SYN
  else if(!strcmp(tok->spelling(),"#SYN")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new SynQNode(1.0, dw);
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getProxChildren(qry);
    unionDocList(qn);
    synonymProxList(qn);
    return qn;
  }
  else {
    // out of vocabulary term. should not be possible here.
    return (QueryNode *)NULL;
  }
}


QueryNode * StructQueryRep::getQryNode(StructQuery &qry, TokenTerm *tok, double w) {

  QueryNode *qn;
  int ti = ind.term(tok->spelling());
  if (ti>0) {
    // found a indexed term
    qn = new TermQnode(ti, w);
    DocInfoList *dl = ind.docInfoList(ti);
    copyDocList(dl, qn);
    delete dl;
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#SUM")) {
    qn = new SumQnode(w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    unionDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#AND")) {
    qn = new AndQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    unionDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#OR")) {
    qn = new OrQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    unionDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#NOT")) {
    qn = new NotQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    // #NOT has to eval every document individually
    int i, dc = ind.docCount()+1;
    bool *didList = new bool[dc];
    for(i = 1; i<=dc; i++)
      didList[i]=true;
    qn->dList=didList;
    qn->nextDoc=1;
    qn->dCnt=ind.docCount();
    qn->proxList=NULL;
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#MAX")) {
    qn = new MaxQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    unionDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#BAND")) {
    qn = new BandQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    intersectDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#BANDNOT")) {
    qn = new BandNotQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    unionDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#FILTREJ")) {
    qn = new FiltRejQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    unionDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#FILTREQ")) {
    qn = new FiltReqQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getChildren(qry);
    intersectDocList(qn);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#WSUM")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new WsumQnode(w);
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getWeightedChildren(qry);
    unionDocList(qn);
    return qn;
  }
  // ODN
  else if(!strcmp(tok->spelling(),"#ODN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new OdnQNode(size, w, dw);
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getProxChildren(qry);
    intersectDocList(qn);
    orderedProxList(qn);
    return qn;
  }
  // UWN
  else if(!strcmp(tok->spelling(),"#UWN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new UwnQNode(size, w, dw);
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getProxChildren(qry);
    intersectDocList(qn);
    unorderedProxList(qn);
    return qn;
  }
  // PASSAGE
  else if(!strcmp(tok->spelling(),"#PASSAGE")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new PassageQNode(size, w);
    qStack[TopqStack++]= qn;
    // passage op allows any kind of children qnodes
    qn->ch=(QnList *)getChildren(qry);
    unionDocList(qn);
    return qn;
  }
  // SYN
  else if(!strcmp(tok->spelling(),"#SYN")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new SynQNode(w, dw);
    qStack[TopqStack++]= qn;
    qn->ch=(QnList *)getProxChildren(qry);
    unionDocList(qn);
    synonymProxList(qn);
    return qn;
  }
  else {
    return (QueryNode *)NULL;
  }
}

StructQueryRep::StructQueryRep(StructQuery &qry, InvFPIndex &dbIndex, double dbelief):
  TopqStack(0), dw(dbelief), ind(dbIndex)
{
  for (int i = 0; i < 100; i++)
    qStack[i] = NULL;
  qry.startTermIteration();
  TokenTerm *tok=qry.nextTerm();
  TopNode=getQryNode(qry, tok, 1.0);
}

