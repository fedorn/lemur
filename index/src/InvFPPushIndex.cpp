/*
 * NAME DATE - COMMENTS
 * tnt 03/2001 - created
 *
 *========================================================================*/
#include "InvFPPushIndex.hpp"

InvFPPushIndex::InvFPPushIndex(char* prefix, int cachesize, DOCID_T startdocid) {
  name = NULL;
  setName(prefix);
  fprintf(stderr, "name is %s\n. ", name);

  cache = new MemCache(cachesize);
  char* docfname = new char[namelen+7];
  sprintf(docfname, "%s.dtlist", name);
  fprintf(stderr, "file name is %s\n.", docfname);
  writetlist = fopen(docfname, "wb");
  //  delete[namelen+7](docfname);
  tcount = tidcount = 0;
}

InvFPPushIndex::~InvFPPushIndex() {
  fclose(writetlist);
  delete(cache);
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
    //update unique word counter for this document
    dtidcount++;
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
  //flush list
  //index of last item in docIDs is current internal docid
  if (dp != NULL) {
    fprintf(writetlist, "%d %d %d ", docIDs.size()-1, dp->length(), termlist.size()*2);
    for (int i=0;i<termlist.size();i++) {
      fprintf(writetlist, "%d %d ", termlist[i].term, termlist[i].loc);
    }
  }
  termlist.clear();  
  //update counts
  tcount += dp->length();
  tidcount += dtidcount;
}

void InvFPPushIndex::endCollection(CollectionProps* cp){
  // flush last time
  // merge temp files

  // write our list of external docids in internal docid order
  writeDocIDs();
  // flush everything in the cache
  lastWriteCache();
  // merge them
  InvFPIndexMerge* merger = new InvFPIndexMerge(&tempfiles);
  merger->merge(name);
}

void InvFPPushIndex::writeDocIDs() {
   char* dname = new char[namelen+4];
   sprintf(dname, "%s.did", name);
   FILE* docid = fopen(dname, "wb");
   for (int i=0;i<docIDs.size();i++) {
     fprintf(docid, "%s\n", docIDs[i]);
   }
   fclose(docid);
   delete[](dname);
}

void InvFPPushIndex::writeCache() {
  char* fname = new char[12];
  sprintf(fname, "tempfile%d", tempfiles.size());
  FILE* write = fopen(fname, "wb");

  if (write == NULL) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    return; 
  }
  tempfiles.push_back(fname);
  
  char* term;
  TABLE_T::iterator finder;
  InvFPDocList* list;
  InvFPDocInfo* info;
  DOCID_T docid = docIDs.size()-1;
  LOC_T* locs;

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
      // don't flush halfway between a document
      // could pose a problem with exceptionally long documents
    //  if (list->curDocID() != docid) {
        // write termid, length of coming list, number of documents this term appeared in
//        fprintf(write, "%s %d %d ", termIDs[i], list->length()+1, list->docFreq());
        fprintf(write, "%d %d %d ", i, list->length()+1, list->docFreq());
        list->startIteration();
        while (list->hasMore()) {
          // as we know this list is of type InvFPDocList, it is pretty safe to assume entries returned are of type InvFPDocInfo
          info = static_cast< InvFPDocInfo* >(list->nextEntry());          

          // write docid, term frequency in this doc, list of positions
          fprintf(write, "%d %d ", info->docID(), info->termCount());
          locs = info->positions();
          for (int j=0;j<info->termCount();j++) {
            fprintf(write, "%d ", locs[j]);
          }
          delete(info);
        } // while list has more docinfo
        // free this list from the cache
        list->reset();
     // } // if not flushing halfway through docid
    } // if needs flushing
  } // for each term
  
  fclose(write);
}

void InvFPPushIndex::lastWriteCache() {
  char* fname = new char[12];
  sprintf(fname, "tempfile%d", tempfiles.size());
  FILE* write = fopen(fname, "wb");

  if (write == NULL) {
    fprintf(stderr, "Can't open file for writing. Cache not written.\n");
    return; 
  }
  tempfiles.push_back(fname);
  
  char* tidname = new char[namelen+4];
  sprintf(tidname, "%s.tid", name);
  FILE* tid = fopen(tidname, "wb");

  char* term;
  TABLE_T::iterator finder;
  InvFPDocList* list;
  InvFPDocInfo* info;
  DOCID_T docid = docIDs.size()-1;
  LOC_T* locs;

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
    fprintf(tid, "%d %d %s\n", i, list->termLen(), term);

    // check to see that there is a list that requires flushing
    if (!list->hasNoMem()) {
      // write termid, length of coming list, number of documents this term appeared in
//      fprintf(write, "%s %d %d ", termIDs[i], list->length()+1, list->docFreq());
      fprintf(write, "%d %d %d ", i, list->length()+1, list->docFreq());
      list->startIteration();
      while (list->hasMore()) {
        // as we know this list is of type InvFPDocList, it is pretty safe to assume entries returned are of type InvFPDocInfo
        info = static_cast< InvFPDocInfo* >(list->nextEntry());          

        // write docid, term frequency in this doc, list of positions
        fprintf(write, "%d %d ", info->docID(), info->termCount());
        locs = info->positions();
        for (int j=0;j<info->termCount();j++) {
          fprintf(write, "%d ", locs[j]);
        }
        delete(info);
      } // while list has more docinfo
      list->reset();
    } // if needs flushing   
  } // for each term
  
  fclose(write);
  fclose(tid);
  delete[](tidname);
}
