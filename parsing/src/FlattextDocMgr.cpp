/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

/** tnt 07/2002 - created
 *  dmf 09/2002 - added parser support
 *  dmf 03/2005 - fix spurious entries in fpos file output at end of input.
 *  dmf 06/2005 - fix handling of filenames with spaces.
 */

#include "FlattextDocMgr.hpp"

lemur::parse::FlattextDocMgr::FlattextDocMgr(const string &name) {
  myparser = NULL;
  entries = NULL;
  open(name);
}

lemur::parse::FlattextDocMgr::FlattextDocMgr(string name, string mode, string source) {
  entries = NULL;
  numdocs = 0;
  readinSources(source);
  // convert mode to lower
  for (int i=0;i<mode.length();i++)
    mode[i] = tolower(mode[i]);
  parseMode = mode;
  myparser = lemur::api::TextHandlerManager::createParser(mode);
  IDname = name;
  // build mode doens't have extension on name
  IDnameext = IDname+FT_SUFFIX;
  prevpos = 0;
}

lemur::parse::FlattextDocMgr::~FlattextDocMgr() {
  if (myparser)
    delete myparser;
  if (entries)
    delete[]entries;
}

bool lemur::parse::FlattextDocMgr::open(const string &manname) {
  // open mode has extension
  IDnameext = manname;
  // strip extension, but this doesn't really get used
  IDname = IDnameext.substr(0, IDnameext.length() - 5);
  return loadTOC(manname);
}

void lemur::parse::FlattextDocMgr::buildMgr() {
  myparser->setTextHandler(this);
  string n = IDname+FT_LOOKUP;
  writefpos.open(n.c_str());
  n = IDname+FT_FID;
  ofstream fid(n.c_str());

  cerr << "Building " << IDname << endl;
  for (int i=0;i<sources.size();i++) {
    fileid = i;
    cerr << "  *Parsing " << sources[i] << endl;
    myparser->parse(sources[i]);
    fid << i << " " << sources[i] << endl;
  }

  writefpos.close();
  fid.close();
  writeTOC();
}

char* lemur::parse::FlattextDocMgr::handleDoc(char* docno) {
  numdocs ++;
  int pos = myparser->getDocBytePos();

  writefpos << docno << " " << fileid << " " << pos << " ";
  prevpos = pos;
  return docno;
}

void lemur::parse::FlattextDocMgr::handleEndDoc() {
  int end = myparser->fileTell();

  // write out number of bytes
  writefpos << end-prevpos << endl;
}

const string &lemur::parse::FlattextDocMgr::getMyID() const{
  return IDnameext;
}

char* lemur::parse::FlattextDocMgr::getDoc(const string &docID) const{
  map<string , lookup_e*, less<string> >::iterator finder;
  finder = table.find(docID);
  if (finder == table.end()) {
    cerr << "This docid " << docID << " was not found." << endl;
    return "";
  }

  lookup_e* e = finder->second;
  char* doc = new char[(*e).bytes +1];
  ifstream read(sources[(*e).fid].c_str(), ios::binary);
  if (!read.is_open()) {
    cerr << "Could not open file " << sources[(*e).fid] << " to get document" << endl;
    return "";
  }
  read.seekg((*e).offset, ios::beg);
  read.read(doc,(*e).bytes);
  read.close();
  // null terminate the string
  doc[(*e).bytes] = '\0';
  return doc;
}


/*=================  PRIVATE  ==========================*/
bool lemur::parse::FlattextDocMgr::readinSources(const string &fn){
  ifstream files(fn.c_str());
  string file;

  if (!files.is_open()) {
    cerr << "Could not open file of sources: " << fn << endl;
    return false;
  }

  while (getline(files, file)) {
    sources.push_back(file);
  }

  files.close();
  return true;
}


void lemur::parse::FlattextDocMgr::writeTOC() {
  string n = IDname+FT_SUFFIX;
  ofstream toc(n.c_str());
  toc << "FILE_LOOKUP " << IDname << FT_LOOKUP << endl;
  toc << "FILE_IDS " << IDname << FT_FID << endl;
  toc << "NUM_FILES " << sources.size() << endl;
  toc << "NUM_DOCS " << numdocs << endl;
  toc << "PARSE_MODE " << parseMode << endl;
  toc.close();
}

bool lemur::parse::FlattextDocMgr::loadTOC(const string &fn) {
  ifstream toc(fn.c_str());
  if (!toc.is_open()) {
    throw lemur::api::Exception ("FlattextDocMgr", "Cannot open TOC file for reading");
    return false;
  }
  string key, val;
  int num = 0;
  string files;
  string lookup;

  std::string line;
  while (getline(toc, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      key = line.substr(0, space);
      val = line.substr(space + 1);
      if (key.compare("FILE_LOOKUP") == 0) 
        lookup = val;
      else if (key.compare("FILE_IDS") == 0)
        files = val;
      else if (key.compare("NUM_DOCS") == 0)
        numdocs = atoi(val.c_str());
      else if (key.compare("NUM_FILES") == 0)
        num = atoi(val.c_str());
      else if (key.compare("PARSE_MODE") == 0) 
        parseMode = val;
    }
  }
  if (!loadFTLookup(lookup)) {
    return false;
  }
  if (!loadFTFiles(files, num)) {
    return false;
  }
  myparser = lemur::api::TextHandlerManager::createParser(parseMode);
  toc.close();
  return true;
}

bool lemur::parse::FlattextDocMgr::loadFTLookup(const string &fn) {
  ifstream lup (fn.c_str());
  if (!lup.is_open()) {
    throw lemur::api::Exception ("FlattextDocMgr", "Cannot open list lookup file");
    return false;
  }
  string docid;
  int fid;
  long pos, bytes;
  entries = new lookup_e[numdocs];
  lookup_e* e;
  int index=0;
  while (lup >> docid >> fid >> pos >> bytes) {
    e = &entries[index];
    index++;
    e->fid = fid;
    e->offset = pos;
    e->bytes = bytes;
    // map does shallow copy
    table[docid] = e;
  }

  lup.close();
  return true;
}

bool lemur::parse::FlattextDocMgr::loadFTFiles(const string &fn, int num) {
  ifstream fns (fn.c_str());
  if (!fns.is_open()) {
    throw lemur::api::Exception ("FlattextDocMgr", "Cannot open list of sources");
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
