/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "InvFPPushIndex.hpp"
#include <sstream>
/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 * tnt 07/2001 - adding [OOV] term and docid
 * tnt 09/2001 - writing more info to disk for load during index open
 * dmf 10/18/2002 -- Add writing of compressed TermInfoLists.
 *
 *========================================================================*/

InvFPPushIndex::InvFPPushIndex(const string &prefix, int cachesize, long maxfilesize, DOCID_T startdocid) : InvPushIndex(prefix, cachesize, maxfilesize, startdocid) {
}

InvFPPushIndex::~InvFPPushIndex() {
}

bool InvFPPushIndex::addTerm(const Term& t){
  TABLE_T::iterator placehold;
  InvFPDocList* curlist;
  const InvFPTerm* term;

  term = dynamic_cast< const InvFPTerm* >(&t);
  if (!term)
    LEMUR_THROW(LEMUR_RUNTIME_ERROR,"InvFPPushIndex expects a Term of type InvFPTerm.");
  
  if (term->strLength() < 1) {
    cerr << "Trying to add term with string length less than 1.  Term ignored." << endl;
    return false;
  }
  if (term->spelling() == NULL) {
    cerr << "Trying to add null term.  Term ignored. " << endl;
    return false;
  }

  //search to see if this is a new term 
  string spell = term->spelling();
  placehold = wordtable.find(spell);
  if (placehold != wordtable.end()) {
    //* OLD WORD *//
    curlist = (InvFPDocList*) placehold->second;

    // the list might not have any memory if it's been flushed since we last saw this word
    if (curlist->hasNoMem()) {
      curlist->allocMem();
    }
    // try to add position information for this current doc to term's doclist
    // we can't depend on our own count for position info because stopwords aren't added
    if (!curlist->addLocation( docIDs.size(), term->position() )) {
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addLocation(docIDs.size(), term->position());
    }

  } else {
    //* NEW WORD *//
    // update unique word counter
    tidcount++;
    //store new word in list of ids
    TERM_T spell = term->spelling();
    termIDs.push_back(spell);

    curlist = new InvFPDocList(cache, termIDs.size(), term->strLength(), docIDs.size(), term->position() );

    if (curlist->hasNoMem()) {
      //we didn't allocate memory in the constructor!
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addLocation(docIDs.size(), term->position());
    }
    wordtable[spell] = curlist;
  }

  // update termlist
  LocatedTerm lt;
  lt.loc = term->position();
  lt.term = curlist->termID();
  termlist.push_back(lt);

  return true;
}

void InvFPPushIndex::endCollection(const CollectionProps* cp){
  // flush last time
  // merge temp files

  // flush everything in the cache
  lastWriteCache();
  delete(cache);
  cache = NULL;

  // write our list of external docids in internal docid order
  writeDocIDs();
  // write our list of dt index files in internal fid order
  writeDTIDs();
  // write list of document managers
  writeDocMgrIDs();

  // merge them
  InvFPIndexMerge* merger = new InvFPIndexMerge((char*)membuf, membufsize,maxfile);
  int numinv = merger->merge(&tempfiles, name);
  delete(merger);

  //write out the main toc file
  writeTOC(numinv);

}

/*===============================================================================
 *  PRIVATE METHODS
 *=============================================================================*/
void InvFPPushIndex::writeTOC(int numinv) {
  string fname = name + INVFPTOC;
  fprintf(stderr, "Writing out main stats table\n");
  FILE* toc = fopen(fname.c_str(), "wb");
  if (!toc) {
    fprintf(stderr, "Could not open .toc file for writing.\n");
    return;
  }
  fprintf(toc, "%s %s\n", VERSION_PAR, IND_VERSION);
  fprintf(toc, "%s  %d\n", NUMDOCS_PAR, docIDs.size());
  fprintf(toc, "%s  %d\n", NUMTERMS_PAR, tcount);
  fprintf(toc, "%s  %d\n", NUMUTERMS_PAR, tidcount);
  fprintf(toc, "%s  %d\n", AVEDOCLEN_PAR, tcount/docIDs.size());
  fprintf(toc, "%s  %s%s\n", INVINDEX_PAR, name.c_str(), INVFPINDEX);
  fprintf(toc, "%s  %d\n", NUMINV_PAR, numinv); 
  fprintf(toc, "%s  %s%s\n", INVLOOKUP_PAR, name.c_str(), INVLOOKUP);
  fprintf(toc, "%s  %s%s\n", DTINDEX_PAR, name.c_str(), DTINDEX);
  fprintf(toc, "%s  %d\n", NUMDT_PAR, dtfiles.size());
  fprintf(toc, "%s  %s%s\n", DTLOOKUP_PAR, name.c_str(), DTLOOKUP);
  fprintf(toc, "%s  %s%s\n", DOCIDMAP_PAR, name.c_str(), DOCIDMAP);
  fprintf(toc, "%s  %s%s\n", TERMIDMAP_PAR, name.c_str(), TERMIDMAP);
  fprintf(toc, "%s  %s%s\n", DOCMGR_PAR, name.c_str(), DOCMGRMAP);

   fclose(toc);
}


void InvFPPushIndex::doendDoc(const DocumentProps* dp, int mgrid){
  //flush list and write to lookup table
  if (dp != NULL) {
    DOCID_T docid = docIDs.size();
    COUNT_T len = dp->length();
    COUNT_T tls = termlist.size();
    
    // make sure the ftell is correct
    writetlist.flush();
    long offset = (long)writetlist.tellp();

    if (offset+(3*sizeof(LOC_T))+(tls*sizeof(LocatedTerm)) > maxfile) {
      writetlist.close();
      std::stringstream nameStr;
      nameStr << name << DTINDEX << dtfiles.size();
      string docfname = nameStr.str();
      dtfiles.push_back(docfname);
      writetlist.open(docfname.c_str(), ios::binary | ios::out);
      offset = 0;
    }

    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset, len, mgrid);
    InvFPTermList *tlist = new InvFPTermList(docid, len, termlist);
    tlist->binWriteC(writetlist);
    delete tlist;
    tcount += len;
  }  
  termlist.clear();  
}

