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
 */

#include "FlattextDocMgr.hpp"

FlattextDocMgr::FlattextDocMgr(const char* name) {
  open(name);
}

FlattextDocMgr::FlattextDocMgr(char* name, ParseModes mode, char* source) {
  numdocs = 0;
  readinSources(source);
  pm = mode;
  myparser = createParser(mode);
  IDname = name;
  prevpos = 0;
}

FlattextDocMgr::~FlattextDocMgr() {
  delete (myparser);
}

bool FlattextDocMgr::open(const char* manname) {
  char* temp = strdup(manname);
  IDname = temp;
  return loadTOC(temp);
}

void FlattextDocMgr::buildMgr() {
  myparser->setTextHandler(this);
  // dangerous but I KNOW that my parser is also a texthandler
  //  TextHandler *th = (TextHandler*)myparser;
  // th->setTextHandler(this);
  string n = IDname+FT_LOOKUP;
  writefpos.open(n.c_str());
  n = IDname+FT_FID;
  ofstream fid(n.c_str());

  long one = 1;
  lastid = -1;
  for (int i=0;i<sources.size();i++) {
    fileid = i;
    myparser->parse((char*)sources[i].c_str());
    fid << i << " " << sources[i] << endl;

    /* we need to get position at end of last doc.  can't really do it 
    /  using parser. also want to avoid doing system specific calls for 
    /  platform compatibilirty issues.
    /  we hope there's no junk from end of last doc to end of file
    /  even if there is running through the parser should ignore it
    */
    ifstream check(sources[i].c_str());
    check.seekg(0, ios::end);
    writefpos << check.tellg() - prevpos-one << endl;
    check.close();
  }

  writefpos.close();
  fid.close();
  writeTOC();
}

char* FlattextDocMgr::handleDoc(char* docno) {
  long one = 1;
  numdocs ++;
  int pos = myparser->getDocBytePos();

  /// don't write out bytelength if it's the first doc in a new file
  if (lastid == fileid)
    writefpos << pos-prevpos-one << endl;

  writefpos << docno << " " << fileid << " " << pos << " ";
  prevpos = pos;
  lastid = fileid;

  return docno;
}

const char* FlattextDocMgr::getMyID() {
  if (IDname.find(FT_SUFFIX) == -1) {
    string s = IDname+FT_SUFFIX;
    return s.c_str();
  }
  return IDname.c_str();
}

char* FlattextDocMgr::getDoc(const char* docID) {
  map<char*, lookup_e, abc>::iterator finder;
  finder = table.find((char*)docID);
  if (finder == table.end()) {
    cerr << "This docid " << docID << " was not found." << endl;
    return "";
  }

  lookup_e e = finder->second;
  char* doc = (char*) malloc(e.bytes * sizeof(char)+1);
  ifstream read(sources[e.fid].c_str());
  if (!read.is_open()) {
    cerr << "Could not open file " << sources[e.fid] << " to get document" << endl;
    return "";
  }
  read.seekg(e.offset, ios::beg);
  read.read(doc,e.bytes);
  read.close();
  // null terminate the string
  doc[e.bytes+1] = '\0';
  return doc;
}

Parser* FlattextDocMgr::createParser (ParseModes mode) {
  Parser* p;
  switch (mode) {
  case TREC:
    p = new TrecParser();
    break;
  case WEB:
    p = new WebParser();
    break;
  case CHINESE:
    p = new ChineseParser();
    break;
  case CHINESECHAR:
    p = new ChineseCharParser();
    break;
  case ARABIC:
    p = new ArabicParser();
    break;
  default:
    throw Exception("FlattextDocMgr", "Parser Mode unregcognized.");
    break;
  }
  return p;
}

/*=================  PRIVATE  ==========================*/
bool FlattextDocMgr::readinSources(char* fn){
  ifstream files(fn);
  string file;

  if (!files.is_open()) {
    cerr << "Could not open file of sources: " << fn << endl;
    return false;
  }

  while (files >> file) {
    sources.push_back(file);
  }

  files.close();
  return true;
}


void FlattextDocMgr::writeTOC() {
  string n = IDname+FT_SUFFIX;
  ofstream toc(n.c_str());
  toc << "FILE_LOOKUP  " << IDname << FT_LOOKUP << endl;
  toc << "FILE_IDS  " << IDname << FT_FID << endl;
  toc << "NUM_FILES " << sources.size() << endl;
  toc << "NUM_DOCS  " << numdocs << endl;
  toc << "PARSE_MODE  " << pm << endl;
  toc.close();
}

bool FlattextDocMgr::loadTOC(char* fn) {
  ifstream toc(fn);
  if (!toc.is_open()) {
    throw Exception ("FlattextDocMgr", "Cannot open TOC file for reading");
    return false;
  }
  string key, val;
  int num;
  char* files;
  while (toc >> key >> val) {
    if (key.compare("FILE_LOOKUP") == 0) {
      if (!loadFTLookup(val.c_str()))
	return false;
    }
    else if (key.compare("FILE_IDS") == 0)
      files = strdup(val.c_str());
    else if (key.compare("NUM_FILES") == 0)
      num = atoi(val.c_str());
    else if (key.compare("PARSE_MODE") == 0) 
      {
	if (val.compare("0") == 0)
	  pm =TREC;
	else if (val.compare("1") == 0)
	  pm = WEB;
	else if (val.compare("2") == 0)
	  pm = CHINESE;
	else if (val.compare("3") == 0)
	  pm = CHINESECHAR;
	else if (val.compare("4") == 0)
	  pm = ARABIC;

      }
  }
  if (!loadFTFiles(files, num))
    return false;
  myparser = createParser(pm);
  free(files);
  toc.close();

  return true;
}

bool FlattextDocMgr::loadFTLookup(const char* fn) {
  ifstream lup (fn);
  if (!lup.is_open()) {
    throw Exception ("FlattextDocMgr", "Cannot open list lookup file");
    return false;
  }
  string docid;
  int fid;
  long pos, bytes;
  while (lup >> docid >> fid >> pos >> bytes) {
    char* id = strdup(docid.c_str());
    lookup_e* e = new lookup_e;
    e->fid = fid;
    e->offset = pos;
    e->bytes = bytes;
    table[id] = *e;
  }

  lup.close();
  return true;
}

bool FlattextDocMgr::loadFTFiles(const char* fn, int num) {
  ifstream fns (fn);
  if (!fns.is_open()) {
    throw Exception ("FlattextDocMgr", "Cannot open list of sources");
    return false;
  }

  string f;
  int id;
  //  string* arr = new string[num];
  sources.resize(num);
  while (fns >> id >> f) {
    //  arr[id] = f;
    sources[id]=f;
  }

  //  for (int i=0; i<num; i++) 
  //  sources.push_back(arr[i]);
  //delete[] arr;
  fns.close();
  return true;
}
