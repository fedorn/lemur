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

#include "IncFPPushIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * dmf 07/2002 - subclass InvFPPushIndex
 *
 *========================================================================*/

IncFPPushIndex::IncFPPushIndex(char* prefix, int cachesize, 
			       long maxfilesize, DOCID_T startdocid) {
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
  if (tryOpen(name)) {
    char* docfname = dtfiles[dtfiles.size() - 1];
    char* lfname = new char[namelen+strlen(DTLOOKUP)];
    sprintf(lfname, "%s%s", name, DTLOOKUP);
    writetlist.open(docfname, ios::binary | ios::app);
    writetlookup = fopen(lfname, "ab");
    delete[](lfname);
  } else {
    char* docfname = new char[namelen+strlen(DTINDEX)+1];
    char* lfname = new char[namelen+strlen(DTLOOKUP)];
    sprintf(docfname, "%s%s%d", name, DTINDEX, 0);
    sprintf(lfname, "%s%s", name, DTLOOKUP);
    writetlist.open(docfname, ios::binary | ios::out);
    dtfiles.push_back(docfname);
    writetlookup = fopen(lfname, "wb");
    delete[](lfname);
  }
}


bool IncFPPushIndex::tryOpen(char *name) {
  // Check if we have a TOC
  char* fname = new char[namelen+strlen(INVFPTOC)];
  sprintf(fname, "%s%s", name, INVFPTOC);
  if (!readToc(fname)) {
    delete[](fname);
    return false;
  }
  // Load existing dtfile ids to memory (dtfiles)
  // all dtfile ids written at endCollection.
  readDtFileIDs();

  // Read existing inv file ids, rename and push
  // onto tempfiles for final merge.
  readInvFileIDs();
  
  // Load existing docids to memory (docIDs)
  // all docids written at endCollection.
  readDocIDs();
  
  // Load existing termids to memory (termIDs) and 
  // seed wordtable to have an empty list for each.
  // all termids written at endCollection.
  readTermIDs();
  
  // Load existing docmgr ids into memory (docmgrs).
  // curdocmgr set by texthandler call to setDocMgr.
  // all docmgr ids written at endCollection.
  readDocMgrIDs();
  // clean up
  if (dtF) free(dtF);
  if (invfpF) free(invfpF);
  if (didF) free(didF);
  if (tidF) free(tidF);
  if (dmgrF) free(dmgrF);
  delete[](fname);
  return true;
}

bool IncFPPushIndex::readToc(char * fileName) {
  FILE* in = fopen(fileName, "rb");
  if (in == NULL) {
    return false;
  }
  invfpF = dtF = dmgrF = didF = tidF = NULL;
  char key[128];
  char val[128];
  while (!feof(in)) {
    if (fscanf(in, "%s %s", key, val) != 2) continue;
    if (strcmp(key, INVINDEX_PAR) == 0) {
      invfpF = strdup(val);
    } else if (strcmp(key, NUMTERMS_PAR) == 0) {
      tcount = atoi(val);
    } else if (strcmp(key, NUMUTERMS_PAR) == 0) {
      tidcount = atoi(val);
    } else if (strcmp(key, DTINDEX_PAR) == 0) {
      dtF = strdup(val);
    } else if (strcmp(key, TERMIDMAP_PAR) == 0) {
      tidF = strdup(val);
    } else if (strcmp(key, DOCIDMAP_PAR) == 0) {
      didF = strdup(val);
    } else if (strcmp(key, DOCMGR_PAR) == 0) {
      dmgrF = strdup(val);
    } 
  }
  return true;
}


void IncFPPushIndex::readDtFileIDs() {
  FILE* in = fopen(dtF, "rb");
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = new char[len + 1];
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
       continue;
    }
    dtfiles.push_back(str);
  }
  fclose(in);
}

void IncFPPushIndex::readDocMgrIDs() {
  FILE* in = fopen(dmgrF, "r");

  int ind, len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &ind, &len) != 2)
      continue;
    // new docmgr ids are made w/ strdup, ugh, so malloc here.
    str = (char*) malloc(sizeof(char) * (len+1));
    if (fscanf(in, "%s", str) != 1) {
      free(str);
      continue;
    }
    docmgrs.push_back(str);
  }
  fclose(in);
}

void IncFPPushIndex::readInvFileIDs() {
  FILE* in = fopen(invfpF, "rb");
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;
    str = new char[len + 1];
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
       continue;
    }
    // make a new temp name, rename file, push on tempfiles.
    char* fname = new char[8 + namelen];
    sprintf(fname, "%stemp%d", name, tempfiles.size());
    if (rename(str, fname)){
      cerr << "unable to rename: " << str << " to " << fname << endl;
      delete[](str);
      return;
    }
    tempfiles.push_back(fname);
    delete[](str);
  }
  fclose(in);
}

void IncFPPushIndex::readTermIDs() {
  FILE* in = fopen(tidF, "rb");
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;
    // new terms are made w/ strdup, ugh, so malloc here.
    str = (char*) malloc(sizeof(char) * (len+1));
    if (fscanf(in, "%s", str) != 1) {
      free(str);
      continue;
    }
    // skip OOV entry
    if (!strcmp(str, "[OOV]")) {
      free(str);
      continue;
    }
    
    termIDs.push_back(str);
    InvFPDocList* newlist; // set up an empty list for each term.
    newlist = new InvFPDocList(cache, termIDs.size(), len);
    // have to waste the first alloc by invoking a reset();
    // update to add a method to resetFree without zapping term id.
    newlist->reset();
    wordtable[str] = newlist;
  }
  fclose(in);
  cache->flushMem(); // reset the cache to empty.
}

void IncFPPushIndex::readDocIDs() {
  FILE* in = fopen(didF, "rb");
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2)
      continue;
    // new docids are made w/ strdup, ugh, so malloc here.
    str = (char*) malloc(sizeof(char) * (len+1));
    if (fscanf(in, "%s", str) != 1) {
      free(str);
      continue;
    }
    // skip OOV entry
    if (!strcmp(str, "[OOV]")) {
      free(str);
      continue;
    }
    docIDs.push_back(str);
  }
  fclose(in);
}

