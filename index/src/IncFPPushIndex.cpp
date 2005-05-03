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

#include "IncFPPushIndex.hpp"
#include <sstream>
/*
 * NAME DATE - COMMENTS
 * dmf 07/2002 - subclass InvFPPushIndex
 *
 *========================================================================*/

lemur::index::IncFPPushIndex::IncFPPushIndex(const std::string &prefix, int cachesize, 
			       long maxfilesize, lemur::api::DOCID_T startdocid) {
  setName(prefix);
  fprintf(stderr, "building %s\n ", name.c_str());
  
  //  membuf = (int*) malloc(cachesize);
  // use new so it throws an exception if it fails
  membuf = new int[cachesize/sizeof(int)];
  
  membufsize = cachesize;
  cache = new lemur::utility::MemCache(membuf, membufsize);
  tcount = tidcount = 0;
  maxfile = maxfilesize;
  curdocmgr = -1;

  // docterm index stuff
  if (tryOpen(name)) {
    string docfname = dtfiles[dtfiles.size() - 1];
    string lfname = name + DTLOOKUP;
    writetlist.open(docfname.c_str(), ios::binary | ios::in | ios::out);
    writetlist.seekp(0, ios::end);
    writetlookup = fopen(lfname.c_str(), "rb+");
    fseek(writetlookup, 0, SEEK_END);
  } else {
    std::stringstream nameStr;
    nameStr << name << DTINDEX << dtfiles.size();
    string docfname = nameStr.str();
    string lfname = name + DTLOOKUP;
    writetlist.open(docfname.c_str(), ios::binary | ios::out);
    dtfiles.push_back(docfname);
    writetlookup = fopen(lfname.c_str(), "wb");
  }
}


bool lemur::index::IncFPPushIndex::tryOpen(const string &name) {
  // Check if we have a TOC
  string fname = name + INVFPTOC;
  if (!readToc(fname)) {
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
  return true;
}

bool lemur::index::IncFPPushIndex::readToc(const string &fileName) {
  FILE* in = fopen(fileName.c_str(), "rb");
  if (in == NULL) {
    return false;
  }
  char key[128];
  char val[128];
  while (!feof(in)) {
    if (fscanf(in, "%s %s", key, val) != 2) continue;
    if (strcmp(key, INVINDEX_PAR) == 0) {
      invfpF = val;
    } else if (strcmp(key, NUMTERMS_PAR) == 0) {
      tcount = atoi(val);
    } else if (strcmp(key, NUMUTERMS_PAR) == 0) {
      tidcount = atoi(val);
    } else if (strcmp(key, DTINDEX_PAR) == 0) {
      dtF = val;
    } else if (strcmp(key, TERMIDMAP_PAR) == 0) {
      tidF = val;
    } else if (strcmp(key, DOCIDMAP_PAR) == 0) {
      didF = val;
    } else if (strcmp(key, DOCMGR_PAR) == 0) {
      dmgrF = val;
    } 
  }
  return true;
}


void lemur::index::IncFPPushIndex::readDtFileIDs() {
  FILE* in = fopen(dtF.c_str(), "rb");
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = new char[len + 1]; // change to fixed size buffer
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
       continue;
    }
    dtfiles.push_back(str);
    delete[](str);
  }
  fclose(in);
}

void lemur::index::IncFPPushIndex::readDocMgrIDs() {
  FILE* in = fopen(dmgrF.c_str(), "r");

  int ind, len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &ind, &len) != 2)
      continue;
    // new docmgr ids are made w/ strdup, ugh, so malloc here.
    str = new char[(len+1)]; // change to fixed size buffer
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      free(str);
      continue;
    }
    docmgrs.push_back(str);
    delete[](str);
  }
  fclose(in);
}

void lemur::index::IncFPPushIndex::readInvFileIDs() {
  FILE* in = fopen(invfpF.c_str(), "rb");
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
    std::stringstream nameStr;
    nameStr << name << "temp" << tempfiles.size();
    string fname = nameStr.str();
    if (rename(str, fname.c_str())){
      cerr << "unable to rename: " << str << " to " << fname << endl;
      delete[](str);
      return;
    }
    tempfiles.push_back(fname);
    delete[](str);
  }
  fclose(in);
}

void lemur::index::IncFPPushIndex::readTermIDs() {
  FILE* in = fopen(tidF.c_str(), "rb");
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;
    str = new char[(len+1)];
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    // skip OOV entry
    if (!strcmp(str, "[OOV]")) {
      delete[](str);
      continue;
    }
    
    termIDs.push_back(str);
    InvFPDocList* newlist; // set up an empty list for each term.
    newlist = new InvFPDocList(cache, termIDs.size(), len);
    // have to waste the first alloc by invoking a reset();
    // update to add a method to resetFree without zapping term id.
    newlist->reset();
    wordtable[str] = newlist;
    delete[](str);
  }
  fclose(in);
  cache->flushMem(); // reset the cache to empty.
}

void lemur::index::IncFPPushIndex::readDocIDs() {
  FILE* in = fopen(didF.c_str(), "rb");
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2)
      continue;
    str = new char[(len+1)];
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    // skip OOV entry
    if (!strcmp(str, "[OOV]")) {
      delete[](str);
      continue;
    }
    docIDs.push_back(str);
    delete[](str);
  }
  fclose(in);
}

