/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
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

lemur::index::InvFPPushIndex::InvFPPushIndex(const string &prefix, int cachesize, long maxfilesize, lemur::api::DOCID_T startdocid) : InvPushIndex(prefix, cachesize, maxfilesize, startdocid) {
}

lemur::index::InvFPPushIndex::~InvFPPushIndex() {
}

bool lemur::index::InvFPPushIndex::addTerm(const lemur::api::Term& t){
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
    lemur::api::TERM_T spell = term->spelling();
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

void lemur::index::InvFPPushIndex::endCollection(const lemur::parse::CollectionProps* cp){
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
  writeTOC(numinv, cp);

}

/*===============================================================================
 *  PRIVATE METHODS
 *=============================================================================*/
void lemur::index::InvFPPushIndex::writeTOC(int numinv, const lemur::parse::CollectionProps* cp) {
  const lemur::parse::BasicCollectionProps* props = dynamic_cast<const lemur::parse::BasicCollectionProps*>(cp);
  string fname = name + INVFPTOC;
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
  toc << INVINDEX_PAR << " " << name << INVFPINDEX << endl;
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


void lemur::index::InvFPPushIndex::doendDoc(const lemur::parse::DocumentProps* dp, int mgrid){
  //flush list and write to lookup table
  if (dp != NULL) {
    lemur::api::DOCID_T docid = docIDs.size();
    lemur::api::COUNT_T len = dp->length();
    lemur::api::COUNT_T tls = termlist.size();
    
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

    fprintf(writetlookup, "%d %d %d %d %d ", docid, dtfiles.size()-1, offset, len, mgrid);
    InvFPTermList *tlist = new InvFPTermList(docid, len, termlist);
    tlist->binWriteC(writetlist);
    delete tlist;
    tcount += len;
  }  
  termlist.clear();  
}

