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


QnList * StructQueryRep::getChildren(StructQuery &qry, getFunc getFn, 
					bool weigh) {
  TokenTerm *tok;
  QueryNode *qn;
  QueryNode *qParent = qStack[topqStack-1];
  QnList *chlist = new QnList();
  int cnt = 0;
  double wt = 1.0;
  while(qry.hasMore()) {
    tok = qry.nextTerm();
    if(!strcmp(tok->spelling(), "RPAREN")) {
      if(qParent == qStack[--topqStack]) {
	qParent->setEntries(cnt);
	return chlist;
      } else {
	cerr << "StructQueryRep::getChildren: Unmatched Parentheses" << endl;
	exit(1);
      }
    } else {
      if (weigh) {
	// each child has to have a weight and a qnode
	wt = (double)atof(tok->spelling());
	tok = qry.nextTerm();
      }
      qn = (this->*getFn)(qry, tok, wt);
      if (qn) { // OOV returns null node
	chlist->push_back(qn);
	cnt++;
      }
    }
  }
  qParent->setEntries(cnt);
  return chlist;
}

QueryNode * StructQueryRep::getProxQryNode(StructQuery &qry, TokenTerm *tok, 
					   double w) {
  // using a separate function for parsing proximity ops
  // because they allow only proximity children
  QueryNode *qn;
  int ti = ind.term(tok->spelling());
  if (ti > 0) {
    // found a indexed term
    qn = new TermQnode(ti, 1.0);
    DocInfoList *dl = ind.docInfoList(ti);
    int listlen = ind.docCount(ti);
    int tf = ind.termCount(ti);
    qn->copyDocList(listlen, tf, dl, numDocs);
    return qn;
  }
  // ODN
  else if(!strcmp(tok->spelling(),"#ODN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new OdnQNode(size, 1.0, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    // get the list of doc's in which contain all query terms
    qn->updateDocList(numDocs);
    return qn;
  }
  // UWN
  else if(!strcmp(tok->spelling(),"#UWN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new UwnQNode(size, 1.0, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  // SYN
  else if(!strcmp(tok->spelling(),"#SYN")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new SynQNode(1.0, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  // PROP
  else if(!strcmp(tok->spelling(),"#PROP")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new PropQNode(w, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else {
    // found an OOV term
    qn = new TermQnode(0, 1.0);
    qn->copyDocList(0, 0, NULL, numDocs);
    return qn;
  }
}


QueryNode * StructQueryRep::getQryNode(StructQuery &qry, TokenTerm *tok, 
				       double w) {

  QueryNode *qn;
  int ti = ind.term(tok->spelling());
  if (ti > 0) {
    // found a indexed term
    qn = new TermQnode(ti, w);
    DocInfoList *dl = ind.docInfoList(ti);
    int listlen = ind.docCount(ti);
    int tf = ind.termCount(ti);
    qn->copyDocList(listlen, tf, dl, numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#SUM")) {
    qn = new SumQnode(w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#AND")) {
    qn = new AndQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#OR")) {
    qn = new OrQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#NOT")) {
    qn = new NotQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    // #NOT has to eval every document individually
    int i, dc = numDocs + 1;
    bool *didList = new bool[dc];
    for(i = 1; i <= dc; i++)
      didList[i] = true;
    qn->dList = didList;
    qn->nextDoc = 1;
    qn->dCnt = numDocs;
    qn->proxList = NULL;
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#MAX")) {
    qn = new MaxQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#BAND")) {
    qn = new BandQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#BANDNOT")) {
    qn = new BandNotQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#FILTREJ")) {
    qn = new FiltRejQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#FILTREQ")) {
    qn = new FiltReqQnode(dw, w);
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else if(!strcmp(tok->spelling(),"#WSUM")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new WsumQnode(w);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode, true));
    qn->updateDocList(numDocs);
    return qn;
  }
  // ODN
  else if(!strcmp(tok->spelling(),"#ODN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new OdnQNode(size, w, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  // UWN
  else if(!strcmp(tok->spelling(),"#UWN")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new UwnQNode(size, w, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  // PASSAGE
  else if(!strcmp(tok->spelling(),"#PASSAGE")) {
    tok = qry.nextTerm();
    int size = atoi(tok->spelling());
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new PassageQNode(size, w);
    qStack[topqStack++] = qn;
    // passage op allows any kind of children qnodes
    qn->setChildren(getChildren(qry, &StructQueryRep::getQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  // SYN
  else if(!strcmp(tok->spelling(),"#SYN")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new SynQNode(w, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  // PROP
  else if(!strcmp(tok->spelling(),"#PROP")) {
    // skip the next token which must be LPAREN
    tok = qry.nextTerm();
    qn = new PropQNode(w, dw);
    qStack[topqStack++] = qn;
    qn->setChildren(getChildren(qry, &StructQueryRep::getProxQryNode));
    qn->updateDocList(numDocs);
    return qn;
  }
  else {
    // found an OOV term
    qn = NULL;
    return qn;
  }
}

StructQueryRep::StructQueryRep(StructQuery &qry, InvFPIndex &dbIndex, 
			       double dbelief):
  topqStack(0), dw(dbelief), ind(dbIndex) {
  for (int i = 0; i < 100; i++)
    qStack[i] = NULL; /// fix this size assumption.
  numDocs = ind.docCount();
  qry.startTermIteration();
  TokenTerm *tok = qry.nextTerm();
  topNode = getQryNode(qry, tok, 1.0);
}
