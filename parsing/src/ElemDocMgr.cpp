/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


/* 
 * tnt 01/2004 - extension of KeyfileDocMgr to add element database 
 */


#include "ElemDocMgr.hpp"

lemur::parse::ElemDocMgr::ElemDocMgr(const string &name, bool readOnly) {
  _readOnly = readOnly;
  IDnameext = name;
  // strip extension
  IDname = IDnameext.substr(0, IDnameext.length() - ELEM_TOC_LEN);

  if (!loadTOC()) {
    // brand new.
    string val = IDname + BT_LOOKUP;
    doclookup.create( val.c_str() );

    val = IDname + BT_POSITIONS;
    poslookup.create( val.c_str() );
    pm = "trec"; /// bleah fix me
    setParser(lemur::api::TextHandlerManager::createParser());

    val = IDname + ELEM_ELS;
    elements.create( val.c_str());    
  }
  // how many have been parsed already.
  numOldSources = sources.size();
}

lemur::parse::ElemDocMgr::ElemDocMgr(string name, string mode, string source) {
  _readOnly = false;
  IDname = name;
  IDnameext = IDname+ELEM_TOC;
  if (! loadTOC()) {    
    pm = mode;
    setParser(lemur::api::TextHandlerManager::createParser(mode));
    string val = IDname + BT_LOOKUP;
    doclookup.create( val.c_str() );
    val = IDname + BT_POSITIONS;
    poslookup.create( val.c_str() );
    val = IDname + ELEM_ELS;
    elements.create( val.c_str());
  }
  // how many have been parsed already.
  numOldSources = sources.size();
  ifstream files(source.c_str());
  string file;
  if (files.is_open()) {
      // filenames may have spaces.
    while (getline(files, file)) {
        sources.push_back(file);
    }
    files.close();
  } else {
    cerr << "Could not open file of sources: " << source << endl;
  }
}

lemur::parse::ElemDocMgr::~ElemDocMgr() {
  elements.close();
}

char* lemur::parse::ElemDocMgr::handleBeginTag(char* tag, const char* orig, PropertyList* props) {
  if (! ignoreDoc) {
    const Property* prop = NULL;
    prop = props->getProperty("B_ELEM");
    if (prop) {
      map<char*, btl, abc>::iterator point = elemtable.find((char*)prop->getValue());
      if (point == elemtable.end()) {
        // new one
        btl entry;
        entry.fid = fileid;
        entry.offset = myparser->fileTell();
        entry.bytes = 0;
        // table should copy this value
        elemtable[(char*)prop->getValue()] = entry;
      } 
      // else we already have a tag with the same name being created. ignore it.
    }
  }
  return tag;
}

char* lemur::parse::ElemDocMgr::handleEndTag(char* tag, const char* orig, PropertyList* props){
  if (!ignoreDoc) {
    // find the tag
    const Property* prop=NULL;
    prop = props->getProperty("E_ELEM");
    if (prop) {
      char* elname = (char*)prop->getValue();
    
      map<char*, btl, abc>::iterator point = elemtable.find(elname);
      if (point != elemtable.end()) {    
        // got it
        // overwrites previous keys
        char* key = new char[strlen(myDoc)+strlen(elname)+1];
        strcpy(key, myDoc);
        strcat(key, elname);
        // fileTell gives us current file position, don't store actual end tag
        (point->second).bytes = myparser->fileTell()-(point->second).offset-strlen(orig);
        elements.put(key, &(point->second), sizeof(btl));
        delete[]key;
        // remove from table
        elemtable.erase(point);
      }
      // else we didn't get it, then there's nothing to do
    }
  }
  return tag;
}

// caller delete[]
char* lemur::parse::ElemDocMgr::getElement(const char* docid, const char* element) const{
  int actual=0;
  btl elemloc;
  char* key = new char[strlen(docid)+strlen(element)+1];
  strcpy(key, docid);
  strcat(key, element);
  bool gotit = elements.get(key,&elemloc,actual,sizeof(btl));
  delete[]key;

  if (!gotit)
    return NULL;

  // get element from file.  possible to have done this during build time and 
  // just store element in btree. either way needs to do seek at build time or 
  // get time. trade-off and storage space depends on length of elements
  char *elem = new char[elemloc.bytes + 1];  
  ifstream read(sources[elemloc.fid].c_str(), ios::binary);
  if (!read.is_open()) {
    cerr << "Could not open file " << sources[elemloc.fid] 
         << " to get document" << endl;
    return NULL;
  }
  read.seekg(elemloc.offset, ios::beg);
  read.read(elem,elemloc.bytes);
  read.close();
  elem[elemloc.bytes] = '\0';
  return elem;
}

void lemur::parse::ElemDocMgr::writeTOC() {
  if (_readOnly) return;
  string n = IDname + ELEM_TOC;
  ofstream toc(n.c_str());
  toc << "FILE_LOOKUP " << IDname << BT_LOOKUP << endl;
  toc << "POS_LOOKUP " << IDname << BT_POSITIONS << endl;
  toc << "DOC_ELEMENTS " << IDname << ELEM_ELS << endl;
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

bool lemur::parse::ElemDocMgr::loadTOC() {
  string n = IDname + ELEM_TOC;
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
      else if (key.compare("DOC_ELEMENTS") == 0)
        elements.open( val, cacheSize, _readOnly );
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


