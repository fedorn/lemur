/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
 * author: dmf
 * 
*/

#include "MatchInfo.hpp"
#include "InvFPIndex.hpp"
#include "StructQueryRep.hpp"
#include "TextQueryRep.hpp"
#include "DocMgrManager.hpp"
#include "Match.hpp"
#include "DocOffsetParser.hpp"
#include "KeyfileDocMgr.hpp"

MatchInfo *MatchInfo::getMatches(const Index &ind, const Query &qry, 
				 int docID) {
    InvFPTermList *docTerms;
    MatchInfo *matches = new MatchInfo();
    TermInfoList *dt = ind.termInfoList(docID);
    docTerms = dynamic_cast<InvFPTermList *>(dt);
    if (docTerms == NULL) {
      // not an InvFP index return an empty list
      delete(dt);
      return (matches);
    }
    InvFPTerm *nextTerm;

   const DocumentManager *docMgr = ind.docManager(docID);    
    vector<Match> offsets;
    if (docMgr != NULL) {
      const KeyfileDocMgr *kdm = dynamic_cast<const KeyfileDocMgr *>(docMgr);
      if (kdm != NULL) {
	// we have a KeyfileDocMgr, no need to parse anything.
	offsets = kdm->getOffsets(ind.document(docID));
      } else {
	char *rawDoc = NULL;
	Parser *p = NULL;
	DocOffsetParser *dp = NULL;
	rawDoc = docMgr->getDoc(ind.document(docID));
	p = docMgr->getParser();
	dp = new DocOffsetParser(p);
	dp->parseString(rawDoc);
	p->setTextHandler(NULL);
	offsets = dp->getOffsets();
	delete[](rawDoc);
	delete(dp);
      }
    }
    
    // collect up the query terms.
    // is it a TextQuery?
    const TextQuery *tq = dynamic_cast<const TextQuery *>(&qry);
    // or is it a StructQuery?
    const StructQuery *sq = dynamic_cast<const StructQuery *>(&qry);
    // the api should be promoted to Query, as both need it.
    const TokenTerm *t;
    set<int, less<int> > termIDs;
    if (tq != NULL) {// TextQuery
      tq->startTermIteration();
      while (tq->hasMore()) {
	t = tq->nextTerm();
	int tid = ind.term(t->spelling());
	if (tid > 0) {
	  termIDs.insert(tid);
	}
      }      
    } else if (sq != NULL) { //StructQuery
      sq->startTermIteration();
      while (sq->hasMore()) {
	t = sq->nextTerm();
	int tid = ind.term(t->spelling());
	if (tid > 0) {
	  termIDs.insert(tid);
	}
      }
    } else {
      // Augh! error.
      // not a known query type
      //cerr << "Unknown query type.";
      return (matches);
    }
    
    docTerms->startIteration();
    while (docTerms->hasMore()) {
      nextTerm = dynamic_cast<InvFPTerm *>(docTerms->nextEntry());
      int did = nextTerm->id();
      for (set<int, less<int> >::iterator iter = termIDs.begin();
	   iter != termIDs.end(); iter++) {
	int tid = *iter;
	if (tid == did) {
	  //	  vector <LOC_T> *pos = nextTerm->positions();
	  //for (vector <LOC_T>::iterator iter1 = pos->begin();
	  //     iter1 != pos->end(); iter1++) {
	  //int pos1 = *iter1;
	  const int* pos = nextTerm->positions();
	  for (int i=0;i<nextTerm->count();i++) {
	    int pos1 = pos[i];
	    if (offsets.size() > 0) 
	      matches->add(tid, pos1, offsets[pos1].start, offsets[pos1].end);
	    else
	      matches->add(tid, pos1);
	  }
	}
      }
    }
    delete(docTerms);
    matches->sort();
    return matches;
}
