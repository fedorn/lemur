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

#include "InvPassagePushIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * dmf 07/24/2002 subclass for passage indexing.
 *========================================================================*/

InvPassagePushIndex::InvPassagePushIndex(char* prefix, int psgSize,
					 int cachesize, long maxfilesize, 
					 DOCID_T startdocid)
  : InvFPPushIndex(prefix, cachesize, maxfilesize, startdocid),
    passageSize(psgSize), curDocName(NULL) { 
  passageEnd = passageSize/2;
  if (passageSize%2 != 0) passageEnd++; // round up
}

InvPassagePushIndex::~InvPassagePushIndex() { 
  if (curDocName != NULL)
    free(curDocName);
}

void InvPassagePushIndex::setPassageSize(int n) {
  passageSize = n;
  passageEnd = passageSize/2;
  if (passageSize%2 != 0) passageEnd++; // round up
}

bool InvPassagePushIndex::beginDoc(DocumentProps* dp){
  if (dp == NULL)
    return false;
  curDoc = dp;
  if (curDocName != NULL)
    free(curDocName);
  curDocName = strdup(dp->stringID());
  char buf[1024];
  psgCounter = 0;
  sprintf(buf, "%s-%d", dp->stringID(), psgCounter);
  char* id = strdup(buf);
  docIDs.push_back(id);
  dtidcount = 0;
  return true;
}

bool InvPassagePushIndex::addTerm(Term& t){
  TABLE_T::iterator placehold;
  InvFPDocList* curlist;
  InvFPTerm* term;

  term = static_cast< InvFPTerm* >(&t);
  if (term->strLength() < 1) {
    cerr << "Trying to add term with length less than 1.  Term ignored." 
	 << endl;
    return false;
  }
  if (term->spelling() == NULL) {
    cerr << "Trying to add null term.  Term ignored. " << endl;
    return false;
  }
  
  int pos = termlist.size() + 1;
  //search to see if this is a new term 
  placehold = wordtable.find((char*)term->spelling());
  if (placehold != wordtable.end()) {
    //* OLD WORD *//
    curlist = (InvFPDocList*)placehold->second;

    // the list might not have any memory if it's been flushed 
    // since we last saw this word
    if (curlist->hasNoMem()) {
      curlist->allocMem();
    }
    // Passage breaks lose position information, stopwords are not counted.
    if (!curlist->addLocation( docIDs.size(), pos )) {
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addLocation(docIDs.size(), pos);
    }

  } else {
    //* NEW WORD *//
    // update unique word counter
    tidcount++;
    //store new word in list of ids
    char* spell = strdup(term->spelling());
    termIDs.push_back(spell);

    curlist = new InvFPDocList(cache, termIDs.size(), term->strLength(), 
			       docIDs.size(), pos);

    if (curlist->hasNoMem()) {
      //we didn't allocate memory in the constructor!
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addLocation(docIDs.size(), pos);
    }
    wordtable[spell] = curlist;
  }
  // update termlist
  int i;
  LocatedTerm lt;
  lt.loc = pos;
  lt.term = curlist->termID();
  termlist.push_back(lt);
  if (termlist.size() == passageSize) {
    // flush a passage.
    int docid = docIDs.size();
    int tls = termlist.size();
    // stop words will not be counted
    int len = tls;
    // make sure the ftell is correct
    writetlist.flush();
    long offset = (long)writetlist.tellp();
    if (offset+(3*sizeof(int))+(tls*sizeof(LocatedTerm)) > maxfile) {
      writetlist.close();
      char* docfname = new char[namelen+strlen(DTINDEX)+2];
      sprintf(docfname, "%s%s%d", name, DTINDEX, dtfiles.size());
      dtfiles.push_back(docfname);
      writetlist.open(docfname, ios::binary | ios::out);
      offset = 0;
    }
    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset,
	    len, curdocmgr);
    writetlist.write((const char*)&docid, sizeof(DOCID_T));
    writetlist.write((const char*)&len, sizeof(int));
    writetlist.write((const char*)&tls, sizeof(int));
    for (i=0;i<tls;i++) {
      writetlist.write((const char*)&termlist[i], sizeof(LocatedTerm));
    }
    tcount += len;
    termlist.erase(termlist.begin(), termlist.begin() + passageEnd);
    // begin a new passage.
    psgCounter++;
    char buf[1024];
    sprintf(buf, "%s-%d", curDocName, psgCounter);
    char* id = strdup(buf);
    docIDs.push_back(id);
    //update positions.
    for (i = 0; i < termlist.size(); i++) {            
      termlist[i].loc = i + 1;
      char *word = termIDs[termlist[i].term - 1]; // off by one for OOV
      placehold = wordtable.find(word);
      // must be there.
      curlist = (InvFPDocList*)placehold->second;
      if (curlist->hasNoMem()) {
	curlist->allocMem();
      }
      if (!curlist->addLocation(docIDs.size(), termlist[i].loc)) {
	writeCache();
	cache->flushMem();
	curlist->allocMem();
	curlist->addLocation(docIDs.size(), termlist[i].loc);
      }
    }
  }
  return true;
}

void InvPassagePushIndex::doendDoc(DocumentProps* dp, int mgrid) {
  //flush list and write to lookup table
  if (dp != NULL) {
    int docid = docIDs.size();
    //    int len = dp->length(); // stop words will not be counted
    int tls = termlist.size();
    int len = tls;
    // make sure the ftell is correct
    writetlist.flush();
    long offset = (long)writetlist.tellp();

    if (offset+(3*sizeof(int))+(tls*sizeof(LocatedTerm)) > maxfile) {
      writetlist.close();
      char* docfname = new char[namelen+strlen(DTINDEX)+2];
      sprintf(docfname, "%s%s%d", name, DTINDEX, dtfiles.size());
      dtfiles.push_back(docfname);
      writetlist.open(docfname, ios::binary | ios::out);
      offset = 0;
    }

    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset, 
      len, mgrid);

    writetlist.write((const char*)&docid, sizeof(DOCID_T));
    writetlist.write((const char*)&len, sizeof(int));
    writetlist.write((const char*)&tls, sizeof(int));

    for (int i=0;i<tls;i++) {
      writetlist.write((const char*)&termlist[i], sizeof(LocatedTerm));
    }
    tcount += len;
  }  
  termlist.clear();  
}
