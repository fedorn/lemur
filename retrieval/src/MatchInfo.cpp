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
#include "IndriDocMgr.hpp"

MatchInfo *MatchInfo::getMatches(const Index &ind, const Query &qry, 
				 DOCID_T docID) {
  const DocumentManager *docMgr = ind.docManager(docID);    
    MatchInfo *matches = new MatchInfo();
  vector<Match> offsets;

  if (docMgr != NULL) {
    const IndriDocMgr *idm = dynamic_cast<const IndriDocMgr *>(docMgr);
    if (idm != NULL) {
      // we have an indri repository, no need to parse anything.
      offsets = idm->getOffsets(ind.document(docID));
      const TermQuery *tq = dynamic_cast<const TermQuery*>(&qry);
      const Term *t;
      set<TERMID_T, less<TERMID_T> > termIDs;
      if (tq != NULL) {// TextQuery
	tq->startTermIteration();
	while (tq->hasMore()) {
	  t = tq->nextTerm();
	  TERMID_T tid = ind.term(t->spelling());
	  if (tid > 0) {
	    termIDs.insert(tid);
	  }
	}
	for (vector<Match>::iterator iter1 = offsets.begin();
	     iter1 != offsets.end(); iter1++) {
	  TERMID_T did = (*iter1).termid;
	  for (set<TERMID_T, less<TERMID_T> >::iterator iter = termIDs.begin();
	       iter != termIDs.end(); iter++) {
	    TERMID_T tid = *iter;
	    if (tid == did) {
	      matches->add(tid, (*iter1).pos, (*iter1).start, (*iter1).end);
	    }
	  }
	}
      } else {
	return (matches);
      }
      return (matches);
    }
  }  // else we want an InvFP index.

    TermInfoList *dt = ind.termInfoList(docID);
  InvFPTermList *docTerms;
    docTerms = dynamic_cast<InvFPTermList *>(dt);
    if (docTerms == NULL) {
      // not an InvFP index return an empty list
      delete(dt);
      return (matches);
    }

  InvFPTerm *nextTerm;
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
    //    const TextQuery *tq = dynamic_cast<const TextQuery *>(&qry);
    const TermQuery *tq = dynamic_cast<const TermQuery*>(&qry);
    // or is it a StructQuery?
    //const StructQuery *sq = dynamic_cast<const StructQuery *>(&qry);
    // the api should be promoted to Query, as both need it.
    const Term *t;
  set<TERMID_T, less<TERMID_T> > termIDs;
    if (tq != NULL) {// TextQuery
      tq->startTermIteration();
      while (tq->hasMore()) {
	t = tq->nextTerm();
      TERMID_T tid = ind.term(t->spelling());
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
    TERMID_T did = nextTerm->termID();
    for (set<TERMID_T, less<TERMID_T> >::iterator iter = termIDs.begin();
	   iter != termIDs.end(); iter++) {
      TERMID_T tid = *iter;
	if (tid == did) {
	const LOC_T* pos = nextTerm->positions();
	for (COUNT_T i=0;i<nextTerm->count();i++) {
	  LOC_T pos1 = pos[i];
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
