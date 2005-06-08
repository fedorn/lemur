/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


/* 
 * dmf 07/2003 - created (minimalist docmgr for bt inc indexer)
 * tds 09/2003 - modified KeyfileDocMgr to work with keyfile
 */

#include "KeyfileDocMgr.hpp"
//keyref.h
#define MAX_DOCID_LENGTH 512 

lemur::parse::KeyfileDocMgr::KeyfileDocMgr(const string &name, bool readOnly) {
  myDoc = NULL;
  _readOnly = readOnly;
  numdocs = 0;
  ignoreDoc = false;
  IDnameext = name;
  // strip extension
  IDname = IDnameext.substr(0, IDnameext.length() - 4);

  if (!loadTOC()) {
    // brand new.
    string val = IDname + BT_LOOKUP;
    doclookup.create(val);

    val = IDname + BT_POSITIONS;
    poslookup.create(val);
    pm = "trec"; /// bleah fix me
    setParser(lemur::api::TextHandlerManager::createParser());
  }
  // how many have been parsed already.
  numOldSources = sources.size();
}

lemur::parse::KeyfileDocMgr::KeyfileDocMgr(string name, string mode, string source) {
  myDoc = NULL;
  numdocs = 0;
  _readOnly = false;
  ignoreDoc = false;
  IDname = name;
  IDnameext = IDname+BT_TOC;
  if (!loadTOC()) {
    // brand new. create
    pm = mode;
    setParser(lemur::api::TextHandlerManager::createParser(mode));
    string val = IDname + BT_LOOKUP;
    doclookup.create(val);
    val = IDname + BT_POSITIONS;
    poslookup.create(val);
  }
  // how many have been parsed already.
  numOldSources = sources.size();
  // add the new files.
  ifstream files(source.c_str());
  string file;
  if (files.is_open()) {
    while (getline(files, file)) {
      sources.push_back(file);
    }
    files.close();
  } else {
    cerr << "Could not open file of sources: " << source << endl;
  }
}

lemur::parse::KeyfileDocMgr::~KeyfileDocMgr() {
  writeTOC();
  delete[](myDoc);

  poslookup.close();
  doclookup.close();
}

void lemur::parse::KeyfileDocMgr::handleEndDoc() {
  if (!ignoreDoc) {
    int end = myparser->fileTell();
    docEntry.bytes = end - docEntry.offset;
    
    // store into table
    doclookup.put( myDoc, &docEntry, sizeof (btl) );

    // write out byte positions array.
    int numOffs = offsets.size();
    int *offs = new int[numOffs * 2];
    int i = 0;
    for (vector <Match>::iterator iter = offsets.begin();
         iter != offsets.end();
         iter++) {
      offs[i++] = (*iter).start;
      offs[i++] = (*iter).end - (*iter).start;
    }
    // compress it
    unsigned char* comp = new unsigned char[numOffs * sizeof(int) * 2];
    int datalen = (numOffs * 2);
    int compbyte = lemur::utility::RVLCompress::compress_ints(offs, comp, datalen);
    
    poslookup.put( myDoc, comp, compbyte );
    offsets.clear();
    delete[](offs);
    delete[](comp);
  }
  ignoreDoc = false;
}

char* lemur::parse::KeyfileDocMgr::handleDoc(char* docno) {
  docEntry.offset = myparser->getDocBytePos();
  docEntry.fid = fileid;
  doclen = strlen(docno) + 1;
  delete[](myDoc);
  if (doclen > (MAX_DOCID_LENGTH - 1)) {
    doclen = MAX_DOCID_LENGTH;
    cerr << "handleDoc: document id " << docno << " is too long ("
         << doclen << " > " << (MAX_DOCID_LENGTH - 1) << ")" << endl;
    cerr << "truncating " << docno << endl;
  }
  myDoc = new char[doclen];
  strncpy(myDoc, docno, doclen - 1);
  myDoc[doclen - 1] = '\0';
  btl documentLocation;
  int actual;
  if (doclookup.get( myDoc, &documentLocation, actual, sizeof(btl) )) {
    // duplicate document id.
    cerr << "lemur::parse::KeyfileDocMgr::handleDoc: duplicate document id " << myDoc 
         << ". Document will be ignored." << endl;
    ignoreDoc = true;
  } else {
    numdocs++;
  }
  return docno;
}

// caller delete[]s
char *lemur::parse::KeyfileDocMgr::getDoc(const string &docID) const{
  int actual = 0;
  btl documentLocation;
  char tmpdoc[MAX_DOCID_LENGTH];
  const char *docName = docID.c_str();
  int doclen = strlen(docName) + 1;
  if (doclen > (MAX_DOCID_LENGTH - 1)) {
    doclen = MAX_DOCID_LENGTH;
    cerr << "lemur::parse::KeyfileDocMgr::getDoc: document id " << docName << " is too long ("
         << doclen << " > " << (MAX_DOCID_LENGTH - 1) << ")" << endl;
    cerr << "truncating " << docName << endl;
    strncpy(tmpdoc, docID.c_str(), doclen - 1);
    tmpdoc[doclen - 1] = '\0';
    docName = tmpdoc;
  }

  doclookup.get( docName, &documentLocation, actual, sizeof(btl) );

  char *doc = new char[documentLocation.bytes + 1];
  ifstream read(sources[documentLocation.fid].c_str(), ios::binary);
  if (!read.is_open()) {
    cerr << "lemur::parse::KeyfileDocMgr::getDoc: Could not open file " 
         << sources[documentLocation.fid] << " to get document" << endl;
    return NULL;
  }
  read.seekg(documentLocation.offset, ios::beg);
  read.read(doc,documentLocation.bytes);
  read.close();
  doc[documentLocation.bytes] = '\0';
  return doc;
}
vector<lemur::parse::Match> lemur::parse::KeyfileDocMgr::getOffsets(const string &docID) const{
  // reset to empty
  offsets.clear();
  int size = 0;
  unsigned char* data = 0;
  char tmpdoc[MAX_DOCID_LENGTH];
  const char *docName = docID.c_str();
  int doclen = strlen(docName) + 1;
  if (doclen > (MAX_DOCID_LENGTH - 1)) {
    doclen = MAX_DOCID_LENGTH;
    cerr << "lemur::parse::KeyfileDocMgr::getOffsets: document id " << docName 
         << " is too long ("
         << doclen << " > " << (MAX_DOCID_LENGTH - 1) << ")" << endl;
    cerr << "truncating " << docName << endl;
    strncpy(tmpdoc, docID.c_str(), doclen - 1);
    tmpdoc[doclen - 1] = '\0';
    docName = tmpdoc;
  }

  poslookup.get( docName, (char**) &data, size );

  int *buffer = new int[(size * 4)];

  // decompress it
  int len = lemur::utility::RVLCompress::decompress_ints(data, buffer, size);
  offsets.resize(len/2);
  for (int i = 0, j = 0; i < len; i += 2, j++) {
    offsets[j].start = buffer[i];
    offsets[j].end = buffer[i] + buffer[i + 1];
  }
  delete[](buffer);
  delete[] data;
  return offsets;
}

void lemur::parse::KeyfileDocMgr::buildMgr() {
  _readOnly = false;
  myparser->setTextHandler(this);
  cerr << "Building " << IDname << endl;
  for (int i = numOldSources; i < sources.size(); i++) {
    fileid = i;
    cerr << "  *Parsing " << sources[i] << endl;
    myparser->parse(sources[i]);
  }
  writeTOC();
}

void lemur::parse::KeyfileDocMgr::writeTOC() {
  if (_readOnly) return;
  string n = IDname + BT_TOC;
  ofstream toc(n.c_str());
  toc << "FILE_LOOKUP " << IDname << BT_LOOKUP << endl;
  toc << "POS_LOOKUP " << IDname << BT_POSITIONS << endl;
  toc << "PARSE_MODE " <<  pm << endl;
  toc << "FILE_IDS " << IDname << BT_FID << endl;
  toc << "NUM_FILES " << sources.size() << endl;
  toc << "NUM_DOCS " << numdocs << endl;
  toc.close();

  n = IDname + BT_FID;
  ofstream fid(n.c_str());
  for (int i = 0; i < sources.size(); i++) {
    fid << i << " " << sources[i] << endl;
  }
  fid.close();
}

bool lemur::parse::KeyfileDocMgr::loadTOC() {
  string n = IDname + BT_TOC;
  ifstream toc(n.c_str());
  if (!toc.is_open()) {
    return false;
  }
  int cacheSize = 1024 * 1024;
  string key, val;
  int num = 0;
  string files;

  std::string line;
  while (getline(toc, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      key = line.substr(0, space);
      val = line.substr(space + 1);
      if (key.compare("FILE_LOOKUP") == 0)
        doclookup.open( val, cacheSize,  _readOnly );
      else if (key.compare("POS_LOOKUP") == 0)
        poslookup.open( val, cacheSize, _readOnly );
      else if (key.compare("FILE_IDS") == 0)
        files = val;    
      else if (key.compare("NUM_DOCS") == 0)
        numdocs = atoi(val.c_str());
      else if (key.compare("NUM_FILES") == 0)
        num = atoi(val.c_str());
      else if (key.compare("PARSE_MODE") == 0) 
        pm = val;    
    }
  }
  toc.close();
  loadFTFiles(files, num);
  setParser(lemur::api::TextHandlerManager::createParser(pm));
  return true;
}


bool lemur::parse::KeyfileDocMgr::loadFTFiles(const string &fn, int num) {
  ifstream fns (fn.c_str());
  if (!fns.is_open()) {
    return false;
  }
  string line, key, val;
  int id;
  sources.resize(num);
  while (getline(fns, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      key = line.substr(0, space);
      val = line.substr(space + 1);
      id = atoi(key.c_str());
      sources[id]=val;
    }
  }
  fns.close();
  return true;
}
