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
#include <sstream>
/*
 * NAME DATE - COMMENTS
 * tnt 01/2002 - created
 * tnt 06/2002 - added docmgr
 *
 *========================================================================*/

InvPushIndex::InvPushIndex(const string &prefix, int cachesize, 
			   long maxfilesize, DOCID_T startdocid) {
  setName(prefix);
  fprintf(stderr, "building %s\n ", name.c_str());

  membuf = (int*) malloc(cachesize);
  membufsize = cachesize;
  cache = new MemCache(membuf, membufsize);
  tcount = tidcount = 0;
  maxfile = maxfilesize;
  curdocmgr = -1;

  // docterm index stuff
  std::stringstream nameStr;
  nameStr << name << DTINDEX << dtfiles.size();
  string docfname = nameStr.str();
  string lfname = name + DTLOOKUP;
  writetlist.open(docfname.c_str(), ios::binary | ios::out);
  dtfiles.push_back(docfname);
  writetlookup = fopen(lfname.c_str(), "wb");
}

InvPushIndex::~InvPushIndex() {
  writetlist.close();
  fclose(writetlookup);
  if (cache)
    delete(cache);
  free(membuf);
}

void InvPushIndex::setName(const string &prefix) {
  name = prefix;
}

bool InvPushIndex::beginDoc(const DocumentProps* dp){
  if (dp == NULL)
    return false;
  docIDs.push_back(dp->stringID());
  dtidcount = 0;
  return true;
}

bool InvPushIndex::addTerm(const Term& t){
  TABLE_T::iterator placehold;
  InvDocList* curlist;
  const InvFPTerm* term;

  term = dynamic_cast< const InvFPTerm* >(&t);
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
    string spell = term->spelling();
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

void InvPushIndex::endDoc(const DocumentProps* dp) {
  doendDoc(dp, curdocmgr);
}

void InvPushIndex::endDoc(const DocumentProps* dp, const string &mgr){
  doendDoc(dp, docMgrID(mgr));
}

void InvPushIndex::endCollection(const CollectionProps* cp){
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

void InvPushIndex::setDocManager (const string &mgrID) {
  curdocmgr = docMgrID(mgrID);
}

/*==========================================================================
 *  PRIVATE METHODS
 *==========================================================================*/
void InvPushIndex::writeTOC(int numinv) {
  string fname = name + MAINTOC;
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
  fprintf(toc, "%s  %s%s\n", INVINDEX_PAR, name.c_str(), INVINDEX);
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

void InvPushIndex::writeDocIDs() {
  string dname = name + DOCIDMAP;
  FILE* docid = fopen(dname.c_str(), "wb");
  // first write out the string value for an unknown docid
  fprintf(docid, "%d %d %s ", 0, strlen(INVALID_STR), INVALID_STR);
  for (int i=0;i<docIDs.size();i++) {
    fprintf(docid, "%d %d %s ", i+1, docIDs[i].size(), docIDs[i].c_str());
  }
  fclose(docid);
}

void InvPushIndex::writeDTIDs() {
  string dname = name + DTINDEX;
  FILE* dtid = fopen(dname.c_str(), "wb");
  for (int i=0;i<dtfiles.size();i++) {
    fprintf(dtid, "%d %d %s ", i, dtfiles[i].size(), dtfiles[i].c_str());
  }
  fclose(dtid);
}

void InvPushIndex::writeDocMgrIDs() {
  string dmname = name + DOCMGRMAP;
  FILE* dmid = fopen(dmname.c_str(), "wb");
  for (int i=0;i<docmgrs.size();i++) {
    fprintf(dmid, "%d %d %s ", i, docmgrs[i].size(), docmgrs[i].c_str());
  }
  fclose(dmid);
}

void InvPushIndex::writeCache() {
  bool written = false;   // did we actually write anything?

  std::stringstream nameStr;
  nameStr << name << "temp" << tempfiles.size();
  string fname = nameStr.str();
  ofstream fout;
  fout.open(fname.c_str(), ios::binary | ios::out);

   if (!fout) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    return; 
  }

  tempfiles.push_back(fname);

  TABLE_T::iterator finder;
  InvDocList* list;
  // write the file out in termid order
  for (int i=0;i<termIDs.size();i++) {
    string &term = termIDs[i];
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
    remove(fname.c_str());
    tempfiles.pop_back();
  }
}

void InvPushIndex::lastWriteCache() {
  bool written = false;   // did we actually write anything?
  std::stringstream nameStr;
  nameStr << name << "temp" << tempfiles.size();
  string fname = nameStr.str();
  ofstream fout;
  fout.open(fname.c_str(), ios::binary | ios::out);

  if (!fout) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    return; 
  }
  tempfiles.push_back(fname);
  
  string tidname = name + TERMIDMAP;
  FILE* tid = fopen(tidname.c_str(), "wb");
  fprintf(tid, "%d %d %s ", 0, strlen(INVALID_STR), INVALID_STR);
  TABLE_T::iterator finder;
  InvDocList* list;

  // write the file out in termid order
  for (int i=0;i<termIDs.size();i++) {
    string &term = termIDs[i];

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
    fprintf(tid, "%d %d %s ", i+1, list->termLen(), term.c_str());

    // check to see that there is a list that requires flushing
    if (!list->hasNoMem()) {
      list->binWriteC(fout);
      list->reset();
      written = true;      
    } // if needs flushing   
  } // for each term
  
  fout.close();
  if (!written) {
    cerr << "Last write: No terms written to " << fname << ". Deleting." << 
      endl;
    remove(fname.c_str());
    tempfiles.pop_back();
  }
  fclose(tid);
}


int InvPushIndex::docMgrID(const string &mgrString) {
  for(int i = 0; i < docmgrs.size(); i++) {
    if (mgrString == docmgrs[i]) {
      return i;
    }
  }
  docmgrs.push_back(mgrString);
  return docmgrs.size()-1;
}

void InvPushIndex::doendDoc(const DocumentProps* dp, int mgrid){
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
      std::stringstream nameStr;
      nameStr << name << DTINDEX << dtfiles.size();
      string docfname = nameStr.str();
      dtfiles.push_back(docfname);
      writetlist.open(docfname.c_str(), ios::binary | ios::out);
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

