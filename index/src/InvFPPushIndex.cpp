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

/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 * tnt 07/2001 - adding [OOV] term and docid
 * tnt 09/2001 - writing more info to disk for load during index open
 *
 *========================================================================*/

InvFPPushIndex::InvFPPushIndex(char* prefix, int cachesize, long maxfilesize, DOCID_T startdocid) : InvPushIndex(prefix, cachesize, maxfilesize, startdocid) {
}

InvFPPushIndex::~InvFPPushIndex() {
}

bool InvFPPushIndex::addTerm(Term& t){
  TABLE_T::iterator placehold;
  InvFPDocList* curlist;
  InvFPTerm* term;

  term = static_cast< InvFPTerm* >(&t);
  if (term->strLength() < 1) {
    cerr << "Trying to add term with string length less than 1.  Term ignored." << endl;
    return false;
  }
  if (term->spelling() == NULL) {
    cerr << "Trying to add null term.  Term ignored. " << endl;
    return false;
  }

  //search to see if this is a new term 
  placehold = wordtable.find((char*)term->spelling());
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
    char* spell = strdup(term->spelling());
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

void InvFPPushIndex::endCollection(CollectionProps* cp){
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
  char* fname = new char[namelen+strlen(INVFPTOC)];
  sprintf(fname, "%s%s", name, INVFPTOC);
  //char* fname = strcat(name, INVFPTOC);
     fprintf(stderr, "Writing out main stats table\n");
   FILE* toc = fopen(fname, "wb");
   if (!toc) {
     fprintf(stderr, "Could not open .toc file for writing.\n");
     delete[](fname);
     return;
   }
   fprintf(toc, "%s %s\n", VERSION_PAR, IND_VERSION);
   fprintf(toc, "%s  %d\n", NUMDOCS_PAR, docIDs.size());
   fprintf(toc, "%s  %d\n", NUMTERMS_PAR, tcount);
   fprintf(toc, "%s  %d\n", NUMUTERMS_PAR, tidcount);
   fprintf(toc, "%s  %d\n", AVEDOCLEN_PAR, tcount/docIDs.size());
   fprintf(toc, "%s  %s%s\n", INVINDEX_PAR, name, INVFPINDEX);
   fprintf(toc, "%s  %d\n", NUMINV_PAR, numinv); 
   fprintf(toc, "%s  %s%s\n", INVLOOKUP_PAR, name, INVLOOKUP);
   fprintf(toc, "%s  %s%s\n", DTINDEX_PAR, name, DTINDEX);
   fprintf(toc, "%s  %d\n", NUMDT_PAR, dtfiles.size());
   fprintf(toc, "%s  %s%s\n", DTLOOKUP_PAR, name, DTLOOKUP);
   fprintf(toc, "%s  %s%s\n", DOCIDMAP_PAR, name, DOCIDMAP);
   fprintf(toc, "%s  %s%s\n", TERMIDMAP_PAR, name, TERMIDMAP);
   fprintf(toc, "%s  %s%s\n", DOCMGR_PAR, name, DOCMGRMAP);

   fclose(toc);
   delete[](fname);
}


void InvFPPushIndex::doendDoc(DocumentProps* dp, int mgrid){
  //flush list and write to lookup table
  if (dp != NULL) {
    int docid = docIDs.size();
    int len = dp->length();
    int tls = termlist.size();
    
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

    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset, len, mgrid);

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

