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
  ifstream toc(fileName.c_str());
  if (!toc.is_open()) {
    return false;
  }
  string key, val;
  std::string line;
  while (getline(toc, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      key = line.substr(0, space);
      val = line.substr(space + 1);
      if (key == INVINDEX_PAR) {
        invfpF = val;
      } else if (key == NUMTERMS_PAR) {
        tcount = atoi(val.c_str());
      } else if (key == NUMUTERMS_PAR) {
        tidcount = atoi(val.c_str());
      } else if (key == DTINDEX_PAR) {
        dtF = val;
      } else if (key == TERMIDMAP_PAR) {
        tidF = val;
      } else if (key == DOCIDMAP_PAR) {
        didF = val;
      } else if (key == DOCMGR_PAR) {
        dmgrF = val;
      } 
    }
  }
  return true;
}


void lemur::index::IncFPPushIndex::readDtFileIDs() {
  ifstream in (dtF.c_str());
  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // numbers are ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      value = value.substr(space1 + 1);
      dtfiles.push_back(value);
    }
  }
  in.close();
}

void lemur::index::IncFPPushIndex::readDocMgrIDs() {
  ifstream in (dmgrF.c_str());
  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // numbers are ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      value = value.substr(space1 + 1);
      docmgrs.push_back(value);
    }
  }
  in.close();
}

void lemur::index::IncFPPushIndex::readInvFileIDs() {
  ifstream in (invfpF.c_str());
  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // numbers are ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      value = value.substr(space1 + 1);
      // make a new temp name, rename file, push on tempfiles.
      std::stringstream nameStr;
      nameStr << name << "temp" << tempfiles.size();
      string fname = nameStr.str();
      if (rename(value.c_str(), fname.c_str())) {
        cerr << "unable to rename: " << value << " to " << fname << endl;
        return;
      }
      tempfiles.push_back(fname);
    }
  }
  in.close();
}

void lemur::index::IncFPPushIndex::readTermIDs() {
  ifstream in (tidF.c_str());
  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // first number is ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      value = value.substr(space1 + 1);
      int len = atoi(num2.c_str());

      // skip OOV entry
      if (value == "[OOV]") continue;
      termIDs.push_back(value);
      InvFPDocList* newlist; // set up an empty list for each term.
      newlist = new InvFPDocList(cache, termIDs.size(), len);
      // have to waste the first alloc by invoking a reset();
      // update to add a method to resetFree without zapping term id.
      newlist->reset();
      wordtable[value] = newlist;
    }
  }
  in.close();
  cache->flushMem(); // reset the cache to empty.    
}

void lemur::index::IncFPPushIndex::readDocIDs() {
  ifstream in (didF.c_str());
  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // numbers are ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      value = value.substr(space1 + 1);
      // skip OOV entry
      if (value == "[OOV]") continue;
      docIDs.push_back(value);
    }
  }
  in.close();
}

