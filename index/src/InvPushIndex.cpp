/*==========================================================================
 *
 *  Original source copyright (c) 2002, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#include "InvPushIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * tnt 01/2002 - created
 * tnt 06/2002 - added docmgr
 *
 *========================================================================*/

InvPushIndex::InvPushIndex(char* prefix, int cachesize, long maxfilesize, DOCID_T startdocid) {
  name = NULL;
  setName(prefix);
  fprintf(stderr, "building %s\n ", name);

  membuf = (int*) malloc(cachesize);
  membufsize = cachesize;
  cache = new MemCache(membuf, membufsize);
  tcount = tidcount = 0;
  maxfile = maxfilesize;
  curdocmgr = -1;

  // docterm index stuff
  char* docfname = new char[namelen+strlen(DTINDEX)+1];
  char* lfname = new char[namelen+strlen(DTLOOKUP)];
  sprintf(docfname, "%s%s%d", name, DTINDEX, 0);
  sprintf(lfname, "%s%s", name, DTLOOKUP);
  writetlist.open(docfname, ios::binary | ios::out);
  dtfiles.push_back(docfname);
  writetlookup = fopen(lfname, "wb");
  delete[](lfname);
}

InvPushIndex::~InvPushIndex() {
  writetlist.close();
  fclose(writetlookup);
  if (cache)
    delete(cache);
  free(name);
  free(membuf);
  int i;
  for (i=1;i<termIDs.size(); i++)
    free(termIDs[i]);
  for (i=0;i<tempfiles.size();i++) {
    delete[](tempfiles[i]);
  }
}

void InvPushIndex::setName(char* prefix) {
  if (name != NULL)
    free(name);

  name = strdup(prefix);
  namelen = strlen(name)+1;
}

bool InvPushIndex::beginDoc(DocumentProps* dp){
  if (dp == NULL)
    return false;

  char* id = strdup(dp->stringID());
  docIDs.push_back(id);
  dtidcount = 0;
  return true;
}

bool InvPushIndex::addTerm(Term& t){
  TABLE_T::iterator placehold;
  InvDocList* curlist;
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
    curlist = placehold->second;

    // the list might not have any memory if it's been flushed since we last saw this word
    if (curlist->hasNoMem()) {
      curlist->allocMem();
    }
    // try to add position information for this current doc to term's doclist
    // we can't depend on our own count for position info because stopwords aren't added
    if (!curlist->addTerm( docIDs.size()) ) {
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addTerm(docIDs.size());
    }
    //update doctermlist
    termlist[curlist->termID()] += 1;

  } else {
    //* NEW WORD *//
    // update unique word counter
    tidcount++;
    //store new word in list of ids
    char* spell = strdup(term->spelling());
    termIDs.push_back(spell);

    curlist = new InvDocList(cache, termIDs.size(), term->strLength());
    curlist->addTerm(docIDs.size());

    if (curlist->hasNoMem()) {
      //we didn't allocate memory in the constructor!
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addTerm(docIDs.size());
    }
    wordtable[spell] = curlist;

    //update termlist
    termlist[termIDs.size()] = 1;
  }

  return true;
}

void InvPushIndex::endDoc(DocumentProps* dp) {
  doendDoc(dp, curdocmgr);
}

void InvPushIndex::endDoc(DocumentProps* dp, const char* mgr){
  doendDoc(dp, docMgrID(mgr));
}

void InvPushIndex::endCollection(CollectionProps* cp){
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
  InvIndexMerge* merger = new InvIndexMerge((char*)membuf, membufsize,maxfile);
  int numinv = merger->merge(&tempfiles, name); 
  delete(merger);

  //write out the main toc file
  writeTOC(numinv);

}

void InvPushIndex::setDocManager (const char* mgrID) {
  curdocmgr = docMgrID(mgrID);
}

/*==========================================================================
 *  PRIVATE METHODS
 *==========================================================================*/
void InvPushIndex::writeTOC(int numinv) {
  char* fname = new char[namelen+strlen(MAINTOC)];
  sprintf(fname, "%s%s", name, MAINTOC);
  //char* fname = strcat(name, MAINTOC);
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
   fprintf(toc, "%s  %s%s\n", INVINDEX_PAR, name, INVINDEX);
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

void InvPushIndex::writeDocIDs() {
   char* dname = new char[namelen+strlen(DOCIDMAP)];
   sprintf(dname, "%s%s", name, DOCIDMAP);
   FILE* docid = fopen(dname, "wb");
   // first write out the string value for an unknown docid
   fprintf(docid, "%d %d %s ", 0, strlen(INVALID_STR), INVALID_STR);
   for (int i=0;i<docIDs.size();i++) {
     fprintf(docid, "%d %d %s ", i+1, strlen(docIDs[i]), docIDs[i]);
     free(docIDs[i]);
   }
   fclose(docid);
   delete[](dname);
}

void InvPushIndex::writeDTIDs() {
  char* dname = new char[namelen+strlen(DTINDEX)];
  sprintf(dname, "%s%s", name, DTINDEX);
  FILE* dtid = fopen(dname, "wb");
  for (int i=0;i<dtfiles.size();i++) {
    fprintf(dtid, "%d %d %s ", i, strlen(dtfiles[i]), dtfiles[i]);
    delete[](dtfiles[i]);
  }
  fclose(dtid);
  delete[](dname);
}

void InvPushIndex::writeDocMgrIDs() {
  char* dmname = new char[namelen+strlen(DOCMGRMAP)];
  sprintf(dmname, "%s%s", name, DOCMGRMAP);
  FILE* dmid = fopen(dmname, "wb");
  for (int i=0;i<docmgrs.size();i++) {
    fprintf(dmid, "%d %d %s ", i, strlen(docmgrs[i]), docmgrs[i]);
    free(docmgrs[i]);
  }
  fclose(dmid);
  delete[]dmname;
}

void InvPushIndex::writeCache() {
  bool written = false;   // did we actually write anything?
  char* fname = new char[8 + namelen];
  sprintf(fname, "%stemp%d", name, tempfiles.size());
  ofstream fout;
  fout.open(fname, ios::binary | ios::out);

   if (!fout) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    delete[](fname);
    return; 
  }

  tempfiles.push_back(fname);
  
  char* term;
  TABLE_T::iterator finder;
  InvDocList* list;
  // write the file out in termid order
  for (int i=0;i<termIDs.size();i++) {
    term = termIDs[i];
    finder = wordtable.find(term);
    if (finder == wordtable.end() ) {
      // this really shouldn't happen. means can't find term in table
      // this does happen... stl table might be managing it in a way i don't fully understand.
      // despite the code entering here, it all works fine.
      // tends to enter here with a large data set and a small cache size   
      //      fprintf (stderr, "Weird things are happening.\n");
      continue;
    } else {
      list = finder->second;
    }
    // check to see that there is a list that requires flushing
    if (!list->hasNoMem()) {
      // write the list out
      list->binWriteC(fout);
      list->reset();
      written = true; // at least one entry in temp file.
    } // if needs flushing
  } // for each term
  
  fout.close();
  if (!written) {
    cerr << "No terms written to " << fname << ". Deleting." << endl;
    remove(fname);
    tempfiles.pop_back();
    delete[](fname);
  }
}

void InvPushIndex::lastWriteCache() {
  bool written = false;   // did we actually write anything?
  char* fname = new char[8 + namelen];
  sprintf(fname, "%stemp%d", name, tempfiles.size());
  ofstream fout;
  fout.open(fname, ios::binary | ios::out);

  if (!fout) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    delete[](fname);
    return; 
  }
  tempfiles.push_back(fname);
  
  char* tidname = new char[namelen+strlen(TERMIDMAP)];
  sprintf(tidname, "%s%s", name, TERMIDMAP);
  FILE* tid = fopen(tidname, "wb");
  fprintf(tid, "%d %d %s ", 0, strlen(INVALID_STR), INVALID_STR);
  char* term;
  TABLE_T::iterator finder;
  InvDocList* list;

  // write the file out in termid order
  for (int i=0;i<termIDs.size();i++) {
    term = termIDs[i];

    finder = wordtable.find(term);
    if (finder == wordtable.end() ) {
      // this really shouldn't happen. means can't find term in table
	  // this does happen... stl table might be managing it in a way i don't fully understand.
	  // despite the code entering here, it all works fine.
      // tends to enter here with a large data set and a small cache size   
//      fprintf (stderr, "Weird things are happening.\n");
      continue;
    } else {
      list = finder->second;
    }
    //write out word to term id table
    fprintf(tid, "%d %d %s ", i+1, list->termLen(), term);

    // check to see that there is a list that requires flushing
    if (!list->hasNoMem()) {
      list->binWriteC(fout);
      list->reset();
      written = true;      
    } // if needs flushing   
  } // for each term
  
//  fclose(write);
  fout.close();
  if (!written) {
    cerr << "Last write: No terms written to " << fname << ". Deleting." << 
      endl;
    remove(fname);
    delete[](fname);
    tempfiles.pop_back();
  }
  fclose(tid);
  delete[](tidname);
}


int InvPushIndex::docMgrID(const char* mgr) {
  for (int i=0;i<docmgrs.size();i++) {
    if (!strcmp(mgr, docmgrs[i])) {
      return i;
    }
  }
  docmgrs.push_back(strdup(mgr));
 
  return docmgrs.size()-1;
}

void InvPushIndex::doendDoc(DocumentProps* dp, int mgrid){
  //flush list and write to lookup table
  if (dp != NULL) {
    int docid = docIDs.size();
    int len = dp->length();
    int tls = termlist.size() *2; // for each item we will write 2 ints

    // make sure the ftell is correct
    writetlist.flush();
    long offset = (long)writetlist.tellp();

    if (offset+(3*sizeof(int))+(tls*sizeof(int)) > maxfile) {
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

    // write our termlist out.. buffering taken care of by os
    for (map<int, int>::iterator look=termlist.begin();look!=termlist.end();look++) {
      writetlist.write((const char*)&(look->first), sizeof(int));
      writetlist.write((const char*)&(look->second), sizeof(int));
    }

    tcount += len;
  }
  termlist.clear();
}

