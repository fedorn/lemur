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

lemur::index::InvPushIndex::InvPushIndex(const string &prefix, int cachesize, 
                                         long maxfilesize, lemur::api::DOCID_T startdocid) {
  setName(prefix);
  fprintf(stderr, "building %s\n ", name.c_str());

  membuf = (int*) malloc(cachesize);
  membufsize = cachesize;
  cache = new lemur::utility::MemCache(membuf, membufsize);
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

lemur::index::InvPushIndex::~InvPushIndex() {
  writetlist.close();
  fclose(writetlookup);
  if (cache)
    delete(cache);
  free(membuf);
}

void lemur::index::InvPushIndex::setName(const string &prefix) {
  name = prefix;
}

bool lemur::index::InvPushIndex::beginDoc(const lemur::parse::DocumentProps* dp){
  if (dp == NULL)
    return false;
  docIDs.push_back(dp->stringID());
  dtidcount = 0;
  return true;
}

bool lemur::index::InvPushIndex::addTerm(const lemur::api::Term& t){
  TABLE_T::iterator placehold;
  InvDocList* curlist;
  if (t.spelling() == NULL) {
    cerr << "Trying to add null term.  Term ignored. " << endl;
    return false;
  }

  string spell = t.spelling();

  if (spell.length() < 1) {
    cerr << "Trying to add term with string length less than 1.  Term ignored." << endl;
    return false;
  }

  //search to see if this is a new term 
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
    termIDs.push_back(spell);

    curlist = new InvDocList(cache, termIDs.size(), spell.length());
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

void lemur::index::InvPushIndex::endDoc(const lemur::parse::DocumentProps* dp) {
  doendDoc(dp, curdocmgr);
}

void lemur::index::InvPushIndex::endDoc(const lemur::parse::DocumentProps* dp, const string &mgr){
  doendDoc(dp, docMgrID(mgr));
}

void lemur::index::InvPushIndex::endCollection(const lemur::parse::CollectionProps* cp){
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
  writeTOC(numinv, cp);

}

void lemur::index::InvPushIndex::setDocManager (const string &mgrID) {
  curdocmgr = docMgrID(mgrID);
}

/*==========================================================================
 *  PRIVATE METHODS
 *==========================================================================*/
void lemur::index::InvPushIndex::writeTOC(int numinv, const lemur::parse::CollectionProps* cp) {
  const lemur::parse::BasicCollectionProps* props = dynamic_cast<const lemur::parse::BasicCollectionProps*>(cp);
  string fname = name + MAINTOC;
  ofstream toc(fname.c_str());
  if (!toc.is_open()) {
    fprintf(stderr, "Could not open .toc file for writing.\n");
    return;
  }
  toc << VERSION_PAR << " " << IND_VERSION << endl;
  toc << NUMDOCS_PAR << " " << docIDs.size() << endl;
  toc << NUMTERMS_PAR << " " << tcount << endl;
  toc << NUMUTERMS_PAR << " " << tidcount << endl;
  toc << AVEDOCLEN_PAR << " " << tcount/docIDs.size() << endl;
  toc << INVINDEX_PAR << " " << name << INVINDEX << endl;
  toc << NUMINV_PAR << " " << numinv << endl; 
  toc << INVLOOKUP_PAR << " " << name << INVLOOKUP << endl;
  toc << DTINDEX_PAR << " " << name <<  DTINDEX << endl;
  toc << NUMDT_PAR << " " << dtfiles.size() << endl;
  toc << DTLOOKUP_PAR << " " << name <<  DTLOOKUP << endl;
  toc << DOCIDMAP_PAR << " " << name << DOCIDMAP << endl;
  toc << TERMIDMAP_PAR << " " << name << TERMIDMAP << endl;
  toc << DOCMGR_PAR << " " << name << DOCMGRMAP << endl;

  if (props) {
    const lemur::parse::Property* p = NULL;
    string value;
    props->startIteration();
    while (props->hasMore()) {
      p = props->nextEntry();
      if (p->getType() == lemur::parse::Property::STDSTRING)
        toc << p->getName() << " " << *(string*)p->getValue() << endl;
      else if (p->getType() == lemur::parse::Property::STRING)
        toc << p->getName() << " " << (char*)p->getValue() << endl;
    }
  }

  toc.close();
}

void lemur::index::InvPushIndex::writeDocIDs() {
  string dname = name + DOCIDMAP;
  FILE* docid = fopen(dname.c_str(), "wb");
  // first write out the string value for an unknown docid
  fprintf(docid, "%d %d %s\n", 0, strlen(INVALID_STR), INVALID_STR);
  for (lemur::api::DOCID_T i=0;i<docIDs.size();i++) {
    fprintf(docid, "%d %d %s\n", i+1, docIDs[i].size(), docIDs[i].c_str());
  }
  fclose(docid);
}

void lemur::index::InvPushIndex::writeDTIDs() {
  string dname = name + DTINDEX;
  FILE* dtid = fopen(dname.c_str(), "wb");
  for (int i=0;i<dtfiles.size();i++) {
    fprintf(dtid, "%d %d %s\n", i, dtfiles[i].size(), dtfiles[i].c_str());
  }
  fclose(dtid);
}

void lemur::index::InvPushIndex::writeDocMgrIDs() {
  string dmname = name + DOCMGRMAP;
  FILE* dmid = fopen(dmname.c_str(), "wb");
  for (int i=0;i<docmgrs.size();i++) {
    fprintf(dmid, "%d %d %s\n", i, docmgrs[i].size(), docmgrs[i].c_str());
  }
  fclose(dmid);
}

void lemur::index::InvPushIndex::writeCache() {
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
  for (lemur::api::TERMID_T i=0;i<termIDs.size();i++) {
    lemur::api::TERM_T &term = termIDs[i];
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

void lemur::index::InvPushIndex::lastWriteCache() {
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
  fprintf(tid, "%d %d %s\n", 0, strlen(INVALID_STR), INVALID_STR);
  TABLE_T::iterator finder;
  InvDocList* list;

  // write the file out in termid order
  for (lemur::api::TERMID_T i=0;i<termIDs.size();i++) {
    lemur::api::TERM_T &term = termIDs[i];

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
    fprintf(tid, "%d %d %s\n", i+1, list->termLen(), term.c_str());

    // check to see that there is a list that requires flushing
    if (!list->hasNoMem()) {
      list->binWriteC(fout);
      // done with this list forever, delete it.
      delete(list);
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


int lemur::index::InvPushIndex::docMgrID(const string &mgrString) {
  for(int i = 0; i < docmgrs.size(); i++) {
    if (mgrString == docmgrs[i]) {
      return i;
    }
  }
  docmgrs.push_back(mgrString);
  return docmgrs.size()-1;
}

void lemur::index::InvPushIndex::doendDoc(const lemur::parse::DocumentProps* dp, int mgrid){
  //flush list and write to lookup table
  if (dp != NULL) {
    lemur::api::DOCID_T docid = docIDs.size();
    lemur::api::COUNT_T len = dp->length();
    lemur::api::COUNT_T tls = termlist.size() *2; // for each item we will write 2 ints

    // make sure the ftell is correct
    writetlist.flush();
    long offset = (long)writetlist.tellp();

    if (offset+(3*sizeof(lemur::api::LOC_T))+(tls*sizeof(lemur::api::LOC_T)) > maxfile) {
      writetlist.close();
      std::stringstream nameStr;
      nameStr << name << DTINDEX << dtfiles.size();
      string docfname = nameStr.str();
      dtfiles.push_back(docfname);
      writetlist.open(docfname.c_str(), ios::binary | ios::out);
      offset = 0;
    }

    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset, len, mgrid);

    writetlist.write((const char*)&docid, sizeof(lemur::api::DOCID_T));
    writetlist.write((const char*)&len, sizeof(lemur::api::COUNT_T));
    writetlist.write((const char*)&tls, sizeof(lemur::api::COUNT_T));

    // write our termlist out.. buffering taken care of by os
    for (map<lemur::api::TERMID_T, lemur::api::COUNT_T>::iterator look=termlist.begin();look!=termlist.end();look++) {
      writetlist.write((const char*)&(look->first), sizeof(lemur::api::TERMID_T));
      writetlist.write((const char*)&(look->second), sizeof(lemur::api::COUNT_T));
    }

    tcount += len;
  }
  termlist.clear();
}

