/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "IncPassagePushIndex.hpp"
#include <sstream>
/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 * tnt 07/2001 - adding [OOV] term and docid
 * tnt 09/2001 - writing more info to disk for load during index open
 * dmf 07/24/2002 subclass for passage indexing.
 * dmf 10/22/2002 -- Add writing of compressed TermInfoLists.
 *========================================================================*/

lemur::index::IncPassagePushIndex::IncPassagePushIndex(const string &prefix, int psgSize,
                                                       int cachesize, long maxfilesize)
  : IncFPPushIndex(prefix, cachesize, maxfilesize),
    passageSize(psgSize), curDocName("") { 
  passageEnd = passageSize/2;
  if (passageSize%2 != 0) passageEnd++;
}

lemur::index::IncPassagePushIndex::~IncPassagePushIndex() { 
}

void lemur::index::IncPassagePushIndex::setPassageSize(int n) {
  passageSize = n;
  passageEnd = passageSize/2;
  if (passageSize%2 != 0) passageEnd++;
}

bool lemur::index::IncPassagePushIndex::beginDoc(const lemur::parse::DocumentProps* dp){
  if (dp == NULL)
    return false;
  //  curDoc = dp;
  curDocName = dp->stringID();
  char buf[1024];
  psgCounter = 0;
  sprintf(buf, "%s-%d", dp->stringID(), psgCounter);
  string id = buf;
  docIDs.push_back(id);
  dtidcount = 0;
  return true;
}

bool lemur::index::IncPassagePushIndex::addTerm(const lemur::api::Term& t){
  TABLE_T::iterator placehold;
  InvFPDocList* curlist;
  const InvFPTerm* term;

  term = dynamic_cast< const InvFPTerm* >(&t);
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
  string spell = term->spelling();
  placehold = wordtable.find(spell);
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
    string spell = term->spelling();
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
    if (offset+(3*sizeof(lemur::api::LOC_T))+(tls*sizeof(LocatedTerm)) > maxfile) {
      writetlist.close();
      std::stringstream nameStr;
      nameStr << name << DTINDEX << dtfiles.size();
      string docfname = nameStr.str();
      dtfiles.push_back(docfname);
      writetlist.open(docfname.c_str(), ios::binary | ios::out);
      offset = 0;
    }
    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset,
            len, curdocmgr);
    InvFPTermList *tlist = new InvFPTermList(docid, len, termlist);
    tlist->binWriteC(writetlist);
    delete tlist;
    tcount += len;
    termlist.erase(termlist.begin(), termlist.begin() + passageEnd);
    // begin a new passage.
    psgCounter++;
    char buf[1024];
    sprintf(buf, "%s-%d", curDocName.c_str(), psgCounter);
    string id = buf;
    docIDs.push_back(id);
    //update positions.
    for (i = 0; i < termlist.size(); i++) {            
      termlist[i].loc = i + 1;
      // off by one for OOV
      string &word = termIDs[termlist[i].term - 1];
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

void lemur::index::IncPassagePushIndex::doendDoc(const lemur::parse::DocumentProps* dp, int mgrid) {
  //flush list and write to lookup table
  if (dp != NULL) {
    lemur::api::DOCID_T docid = docIDs.size();
    //    int len = dp->length(); // stop words will not be counted
    lemur::api::COUNT_T tls = termlist.size();
    lemur::api::COUNT_T len = tls;
    // make sure the ftell is correct
    writetlist.flush();
    long offset = (long)writetlist.tellp();

    if (offset+(3*sizeof(lemur::api::LOC_T))+(tls*sizeof(LocatedTerm)) > maxfile) {
      writetlist.close();
      std::stringstream nameStr;
      nameStr << name << DTINDEX << dtfiles.size();
      string docfname = nameStr.str();
      dtfiles.push_back(docfname);
      writetlist.open(docfname.c_str(), ios::binary | ios::out);
      offset = 0;
    }

    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset, 
            len, mgrid);
    InvFPTermList *tlist = new InvFPTermList(docid, len, termlist);
    tlist->binWriteC(writetlist);
    delete tlist;
    tcount += len;
  }  
  termlist.clear();  
}
