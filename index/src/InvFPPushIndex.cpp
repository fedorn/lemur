
#include "InvFPPushIndex.hpp"
/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/

InvFPPushIndex::InvFPPushIndex(char* prefix, int cachesize, long maxfilesize, DOCID_T startdocid) {
  name = NULL;
  setName(prefix);
  fprintf(stderr, "building %s\n ", name);

  cache = new MemCache(cachesize);
  char* docfname = new char[namelen+strlen(DTINDEX)+1];
  char* lfname = new char[namelen+strlen(DTLOOKUP)];
  sprintf(docfname, "%s%s%d", name, DTINDEX, 0);
  sprintf(lfname, "%s%s", name, DTLOOKUP);
//  fprintf(stderr, "file name is %s\n.", docfname);
  //writetlist = fopen(docfname, "wb");
  writetlist.open(docfname, ios::binary | ios::out);
  dtfiles.push_back(docfname);
  writetlookup = fopen(lfname, "wb");
  delete[](lfname);
  tcount = tidcount = 0;
  maxfile = maxfilesize;
}

InvFPPushIndex::~InvFPPushIndex() {
//  fclose(writetlist);
  writetlist.close();
  delete(cache);
  free(name);
}

void InvFPPushIndex::setName(char* prefix) {
  if (name != NULL)
    free(name);

  name = strdup(prefix);
  namelen = strlen(name)+1;
}

bool InvFPPushIndex::beginDoc(DocumentProps* dp){
  if (dp == NULL)
    return false;

  //strdup here necessary?
  char* id = strdup(dp->stringID());
  docIDs.push_back(id);
  //docIDs.push_back(dp->stringID());
  dtidcount = 0;
  return true;
}

bool InvFPPushIndex::addTerm(Term& t){
  TABLE_T::iterator placehold;
  InvFPDocList* curlist;
  InvFPTerm* term;

  term = static_cast< InvFPTerm* >(&t);

  //search to see if this is a new term 
  placehold = wordtable.find(term->spelling());
  if (placehold != wordtable.end()) {
    //* OLD WORD *//
    curlist = placehold->second;

    // the list might not have any memory if it's been flushed since we last saw this word
    if (curlist->hasNoMem()) {
      curlist->allocMem();
    }
    // try to add position information for this current doc to term's doclist
    // we can't depend on our own count for position info because stopwords aren't added
    if (!curlist->addLocation( docIDs.size()-1, term->position() )) {
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addLocation(docIDs.size()-1, term->position());
    }

  } else {
    //* NEW WORD *//
    // update unique word counter
    tidcount++;
    //store new word in list of ids
    char* spell = strdup(term->spelling());
    curlist = new InvFPDocList(cache, termIDs.size(), term->strLength(), docIDs.size()-1, term->position() );

    if (curlist->hasNoMem()) {
      //we didn't allocate memory in the constructor!
      // flush the cache
      writeCache();
      cache->flushMem();
      // we still need to add this term
      curlist->allocMem();
      curlist->addLocation(docIDs.size()-1, term->position());
    }
    termIDs.push_back(spell);
    wordtable[spell] = curlist;
  }

  // update termlist
  LocatedTerm lt;
  lt.loc = term->position();
  lt.term = curlist->termID();
  termlist.push_back(lt);

  return true;
}

void InvFPPushIndex::endDoc(DocumentProps* dp){
  //flush list and write to lookup table
  //index of last item in docIDs is current internal docid
  if (dp != NULL) {
    int docid = docIDs.size()-1;
    int len = dp->length();
    int tls = termlist.size();
    
    // make sure the ftell is correct
    writetlist.flush();
    long offset = (long)writetlist.tellp();

    if (offset+(3*sizeof(int))+(tls*sizeof(LocatedTerm)) > maxfile) {
      writetlist.close();
      char* docfname = new char[namelen+strlen(DTINDEX)+1];
      sprintf(docfname, "%s%s%d", name, DTINDEX, dtfiles.size());
      dtfiles.push_back(docfname);
      writetlist.open(docfname, ios::binary | ios::out);
      offset = 0;
    }

    fprintf(writetlookup, "%d %d %d ", docid, dtfiles.size()-1, offset);

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

void InvFPPushIndex::endCollection(CollectionProps* cp){
  // flush last time
  // merge temp files

  // write our list of external docids in internal docid order
  writeDocIDs();
  // write our list of dt index files in internal fid order
  writeDTIDs();
  // flush everything in the cache
  lastWriteCache();
  // merge them
  InvFPIndexMerge* merger = new InvFPIndexMerge(&tempfiles);
  merger->merge(name);
  delete(merger);

  //write out the main toc file
  writeTOC();

}

/*==================================================================================
 *  PRIVATE METHODS
 *==================================================================================*/
void InvFPPushIndex::writeTOC() {
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
   fprintf(toc, "%s: %d\n", NUMDOCS_PAR, docIDs.size());
   fprintf(toc, "%s: %d\n", NUMTERMS_PAR, tcount);
   fprintf(toc, "%s: %d\n", NUMUTERMS_PAR, tidcount);
   fprintf(toc, "%s: %d\n", AVEDOCLEN_PAR, tcount/docIDs.size());
   fprintf(toc, "%s: %s%s\n", INVINDEX_PAR, name, INVINDEX);
   fprintf(toc, "%s: %s%s\n", INVLOOKUP_PAR, name, INVLOOKUP);
   fprintf(toc, "%s: %s%s\n", DTINDEX_PAR, name, DTINDEX);
   fprintf(toc, "%s: %d\n", NUMDT_PAR, dtfiles.size());
   fprintf(toc, "%s: %s%s\n", DTLOOKUP_PAR, name, DTLOOKUP);
   fprintf(toc, "%s: %s%s\n", DOCIDMAP_PAR, name, DOCIDMAP);
   fprintf(toc, "%s: %s%s\n", TERMIDMAP_PAR, name, TERMIDMAP);

   fclose(toc);
   delete[](fname);
}

void InvFPPushIndex::writeDocIDs() {
   char* dname = new char[namelen+strlen(DOCIDMAP)];
   sprintf(dname, "%s%s", name, DOCIDMAP);
   FILE* docid = fopen(dname, "wb");
   for (int i=0;i<docIDs.size();i++) {
     fprintf(docid, "%d %d %s ", i, strlen(docIDs[i]), docIDs[i]);
   }
   fclose(docid);
   delete[](dname);
}

void InvFPPushIndex::writeDTIDs() {
  char* dname = new char[namelen+strlen(DTINDEX)];
  sprintf(dname, "%s%s", name, DTINDEX);
  FILE* dtid = fopen(dname, "wb");
  for (int i=0;i<dtfiles.size();i++) {
    fprintf(dtid, "%d %d %s ", i, strlen(dtfiles[i]), dtfiles[i]);
  }
  fclose(dtid);
  delete[](dname);
}

void InvFPPushIndex::writeCache() {
  char* fname = new char[8 + namelen];
  sprintf(fname, "%stemp%d", name, tempfiles.size());
//  FILE* write = fopen(fname, "wb");
  ofstream fout;
  fout.open(fname, ios::binary | ios::out);


//  if (write == NULL) {
   if (!fout) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    return; 
  }

  tempfiles.push_back(fname);
  
  char* term;
  TABLE_T::iterator finder;
  InvFPDocList* list;
//  InvFPDocInfo* info;
  DOCID_T docid = docIDs.size()-1;
//  LOC_T* locs;

  // write the file out in termid order
  for (int i=0;i<termIDs.size();i++) {
    term = termIDs[i];
    finder = wordtable.find(term);
    if (finder == wordtable.end() ) {
      // this really shouldn't happen. means can't find term in table
      fprintf (stderr, "Weird things are happening.\n");
      continue;
    } else {
      list = finder->second;
    }
    // check to see that there is a list that requires flushing
    if (!list->hasNoMem()) {
      // write the list out
      list->binWrite(fout);
      list->reset();
    } // if needs flushing
  } // for each term
  
//  fclose(write);
  fout.close();
}

void InvFPPushIndex::lastWriteCache() {
  char* fname = new char[8 + namelen];
  sprintf(fname, "%stemp%d", name, tempfiles.size());
//  FILE* write = fopen(fname, "wb");
  ofstream fout;
  fout.open(fname, ios::binary | ios::out);

//  if (write == NULL) {
  if (!fout) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    return; 
  }
  tempfiles.push_back(fname);
  
  char* tidname = new char[namelen+strlen(TERMIDMAP)];
  sprintf(tidname, "%s%s", name, TERMIDMAP);
  FILE* tid = fopen(tidname, "wb");

  char* term;
  TABLE_T::iterator finder;
  InvFPDocList* list;
  DOCID_T docid = docIDs.size()-1;

  // write the file out in termid order
  for (int i=0;i<termIDs.size();i++) {
    term = termIDs[i];

    finder = wordtable.find(term);
    if (finder == wordtable.end() ) {
      // this really shouldn't happen. means can't find term in table
      fprintf (stderr, "Weird things are happening.\n");
      continue;
    } else {
      list = finder->second;
    }
    //write out word to term id table
    fprintf(tid, "%d %d %s ", i, list->termLen(), term);

    // check to see that there is a list that requires flushing
    if (!list->hasNoMem()) {
      list->binWrite(fout);
      list->reset();
    } // if needs flushing   
  } // for each term
  
//  fclose(write);
  fout.close();
  fclose(tid);
  delete[](tidname);
}
