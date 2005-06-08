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


#include "InvIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * tnt 04/2002 - created
 * dmf 10/22/2002 -- Change to open all of the inverted list files 
 * (dtfiles, invfiles) once at initialization and store the open streams 
 * in the arrays dtfstreams and invfstreams.
 *
 *========================================================================*/

lemur::index::InvIndex::InvIndex() {
  lookup = NULL;  
  dtlookup = NULL;
  dtfiles = NULL;
  dtfstreams = NULL;
  invfiles = NULL;
  invfstreams = NULL;
  docnames = NULL;
  terms = NULL;
  aveDocLen = 0;
}

lemur::index::InvIndex::InvIndex(const string &indexName) {
  lookup = NULL;  
  dtlookup = NULL;
  dtfiles = NULL;
  dtfstreams = NULL;
  invfiles = NULL;
  invfstreams = NULL;
  docnames = NULL;
  terms = NULL;
  aveDocLen = 0;
  open(indexName);
}

lemur::index::InvIndex::~InvIndex() {
  int i;
  delete[](lookup);
  delete[](dtlookup);
  delete[](names);
  delete[](dtfiles);

  if (dtfstreams != NULL) {
    for (i=0;i<counts[DT_FILES];i++) {
      dtfstreams[i].close();
    }
    delete[](dtfstreams);
  }

  delete[](invfiles);

  if (invfstreams != NULL) {
    for (i=0;i<counts[INV_FILES];i++) {
      invfstreams[i].close();
    }
    delete[](invfstreams);
  }

  for (i = 0; i < docmgrs.size(); i++)
    delete(docmgrs[i]);

  delete[](terms);
  delete[](docnames);
  delete[](counts);
}

bool lemur::index::InvIndex::open(const string &indexName){
  counts = new lemur::api::COUNT_T[5];
  names = new string[NAMES_SIZE];  
  names[VERSION_NUM] = "";
  names[DOCMGR_IDS] = "";

  lemur::utility::String streamSelect = lemur::api::ParamGetString("stream", "cerr");
  if (streamSelect == "cout") {
    setMesgStream(&cout);
  } else {
    setMesgStream(&cerr);
  }

  if (!fullToc(indexName)) {
    cerr << "Couldn't not properly parse param file. Index was not loaded." << endl;
    return false;
  }

  dtloaded = lemur::api::ParamGetInt("loadDT", 1);

  if (!dtloaded) {
    *msgstream << "Parameter set to dtlist lookup not being loaded" << endl;
  } else {

    // older versions didn't specify version name and will be blank
    // if it's new we call new load functions
    if (!names[VERSION_NUM].empty()) {
      if (!dtLookup())
        return false;
      if (!docMgrIDs())
        return false;
    } else {
      if (!dtLookup_ver1())
        return false;
    }

    if (!dtFileIDs())
      return false;
  }

  if (!indexLookup())
    return false;

  if (!invFileIDs())
    return false;

  if (!termIDs())
    return false;

  if (!docIDs())
    return false;

  *msgstream << "Load index complete." << endl;
  return true;
}

lemur::api::TERMID_T lemur::index::InvIndex::term(const lemur::api::TERM_T &word) const{
  map<lemur::api::TERM_T, lemur::api::TERMID_T, less<lemur::api::TERM_T> >::iterator point = termtable.find(word);
  if (point != termtable.end()) 
    return point->second;

  return 0;
}

const lemur::api::TERM_T lemur::index::InvIndex::term(lemur::api::TERMID_T termID) const {
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS]))
    return "";
  return terms[termID];
}

lemur::api::DOCID_T lemur::index::InvIndex::document(const lemur::api::EXDOCID_T &docIDStr) const{
  map<lemur::api::EXDOCID_T, lemur::api::DOCID_T, less<lemur::api::EXDOCID_T> >::iterator point = doctable.find(docIDStr);
  if (point != doctable.end()) 
    return point->second;

  return 0;
}

const lemur::api::EXDOCID_T lemur::index::InvIndex::document(lemur::api::DOCID_T docID) const { 
  if ((docID < 0) || (docID > counts[DOCS]))
    return "";
  return docnames[docID];
}

//const char* lemur::index::InvIndex::docManager(int docID) {
const lemur::api::DocumentManager* lemur::index::InvIndex::docManager(lemur::api::DOCID_T docID) const{
  // no such thing previous to version 1.9
  if (names[VERSION_NUM].empty()) 
    return NULL;

  if ((docID < 0) || (docID > counts[DOCS])) {
    fprintf(stderr, "Error trying to get docManager for invalid docID.\n");
    return NULL;
  }

  // there is no such document
  if (docID == 0)
    return NULL;

  // if it never had one registered.
  if (dtlookup[docID].docmgr == -1)
    return NULL;

  return docmgrs[dtlookup[docID].docmgr]; 
}

lemur::api::COUNT_T lemur::index::InvIndex::termCount(lemur::api::TERMID_T termID) const{
  // the termCount is equal to the position counts
  // this is calculated by the length of the inverted list -(df*2) for each doc, 
  // there is the docid and the tf
  if ((termID <0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get termCount for invalid termID" << endl;
    return 0;
  }

  // this is OOV
  if (termID == 0)
    return 0;

  return lookup[termID].ctf;
}

float lemur::index::InvIndex::docLengthAvg() const{
  return aveDocLen;
}

lemur::api::COUNT_T lemur::index::InvIndex::docCount(lemur::api::TERMID_T termID)  const{
  if ((termID <0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docCount for invalid termID" << endl;
    return 0;
  }

  // this is OOV
  if (termID == 0)
    return 0;

  return lookup[termID].df;
}

lemur::api::COUNT_T lemur::index::InvIndex::docLength(lemur::api::DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get docLength for invalid docID." << endl;
    return 0;
  }
    
  if (docID == 0)
    return 0;

  if (!dtloaded) {
    *msgstream << "DT index must be loaded to obtain docLength" << endl;
    return 0;
  }

  return dtlookup[docID].length;
}

lemur::api::COUNT_T lemur::index::InvIndex::docLengthCounted(lemur::api::DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get docLengthCounted for invalid docID." << endl;
    return 0;
  }
    
  if (docID == 0)
    return 0;

  if (!dtloaded) {
    *msgstream << "DT index must be loaded to obtain docLength" << endl;
    return 0;
  }

  lemur::api::COUNT_T dl;
  lemur::api::DOCID_T id;
  ifstream *look = &(dtfstreams[dtlookup[docID].fileid]);
  if (!look) {
    *msgstream << "Could not open term indexfile for reading." << endl;
    return 0;
  }
  look->seekg(dtlookup[docID].offset, ios::beg);
  look->read((char*)&id, sizeof(lemur::api::DOCID_T));
  look->read((char*)&dl, sizeof(lemur::api::COUNT_T));
  look->read((char*)&dl, sizeof(lemur::api::COUNT_T));
  if ( !(look->gcount() == sizeof(lemur::api::COUNT_T)) ) 
    return 0;
  
  return dl/2;
}

lemur::api::DocInfoList* lemur::index::InvIndex::docInfoList(lemur::api::TERMID_T termID) const{
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docInfoList for invalid termID" << endl;
    return NULL;
  }

  if (termID == 0)
    return NULL;
  ifstream *indexin = &(invfstreams[lookup[termID].fileid]);
  indexin->seekg(lookup[termID].offset, ios::beg);
  lemur::api::DocInfoList* doclist;
  InvDocList* dlist = new InvDocList();
  bool success;

  if (!names[VERSION_NUM].empty()) {
    // version 1.9 is compressed and must be decompressed
    success = dlist->binReadC(*indexin);
  } else {
    success = dlist->binRead(*indexin);
  }
  if (!success) {
    return NULL;
  } else {
    doclist = dlist;
    return doclist;
  }
}

lemur::api::TermInfoList* lemur::index::InvIndex::termInfoList(lemur::api::DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get termInfoList for invalid docID." << endl;
    return NULL;
  }

  if (docID == 0)
    return NULL;

  if (!dtloaded) {
    *msgstream << "DT index must be loaded to obtain termInfoList" << endl;
    return NULL;
  }
  ifstream *indexin = &(dtfstreams[dtlookup[docID].fileid]);
  indexin->seekg(dtlookup[docID].offset, ios::beg);
  lemur::api::TermInfoList* termlist;
  InvTermList* tlist = new InvTermList();
  if (!tlist->binRead(*indexin)) {
    return NULL;
  } 
  termlist = tlist;
  return termlist;
}

/*=======================================================================
 * PRIVATE METHODS 
 =======================================================================*/
bool lemur::index::InvIndex::fullToc(const string &fileName) {
  ifstream in(fileName.c_str());
  *msgstream << "Trying to open toc: " << fileName << endl;
  if (!in.is_open()) {
    *msgstream << "Couldn't open toc file for reading" << endl;
    return false;
  }

  lemur::parse::Property p;

  string key,val;
  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      key = line.substr(0, space);
      val = line.substr(space + 1);
      if (key.compare(NUMDOCS_PAR) == 0) {
        counts[DOCS] = atoi(val.c_str());
      } else if (key.compare(NUMTERMS_PAR) == 0) {
        counts[TOTAL_TERMS] = atoi(val.c_str());
      } else if (key.compare(NUMUTERMS_PAR) == 0) {
        counts[UNIQUE_TERMS] = atoi(val.c_str());
      } else if (key.compare(NUMDT_PAR) == 0) {
        counts[DT_FILES] = atoi(val.c_str());
      } else if (key.compare(NUMINV_PAR) == 0) {
        counts[INV_FILES] = atoi(val.c_str());
      } else if (key.compare(INVINDEX_PAR) == 0) {
        names[DOC_INDEX] = val;
      } else if (key.compare(INVLOOKUP_PAR) == 0) {
        names[DOC_LOOKUP] = val;
      } else if (key.compare(DTINDEX_PAR) == 0) {
        names[TERM_INDEX] = val;
      } else if (key.compare(DTLOOKUP_PAR) == 0) {
        names[TERM_LOOKUP] = val;
      } else if (key.compare(TERMIDMAP_PAR) == 0) {
        names[TERM_IDS] = val;
      } else if (key.compare(DOCIDMAP_PAR) == 0) {
        names[DOC_IDS] = val;
      } else if (key.compare(DOCMGR_PAR) == 0) {
        names[DOCMGR_IDS] = val;
      } else if (key.compare(VERSION_PAR) == 0) {
        names[VERSION_NUM] = val;
      } else {
        p.setName(key);
        p.setValue(val.c_str());
        colprops.setProperty(&p);
      }
    }
  }    

  aveDocLen = counts[TOTAL_TERMS] / (float) counts[DOCS];

  in.close();
  return true;
}

bool lemur::index::InvIndex::indexLookup() {
  FILE* in = fopen(names[DOC_LOOKUP].c_str(), "rb");
  *msgstream << "Trying to open invlist lookup: " << names[DOC_LOOKUP] << endl;
  if (in == NULL) {
    *msgstream << "Couldn't open invlist lookup table for reading" << endl;
    return false;
  }

  lookup = new inv_entry[counts[UNIQUE_TERMS]+1];
  inv_entry* e;
  lemur::api::TERMID_T tid =0;
  lemur::api::FILEID_T fid =0;
  long off =0;
  lemur::api::COUNT_T ctf = 0;
  lemur::api::COUNT_T df = 0;
  // if COUNT_T changes, this needs to be changed...
  while (fscanf(in, "%d %d %d %d %d", &tid, &fid, &off, &ctf, &df) == 5) {
    e = &lookup[tid];
    e->fileid = fid;
    e->offset = off;
    e->ctf = ctf;
    e->df = df;
  }
  fclose(in);
 
  return true;
}

bool lemur::index::InvIndex::dtLookup_ver1() {
  FILE* in = fopen(names[TERM_LOOKUP].c_str(), "rb");
  *msgstream << "Trying to open dtlist lookup: " 
             << names[TERM_LOOKUP] << endl;
  if (in == NULL) {
    *msgstream <<  "Couldn't open dt lookup table for reading" << endl;
    return false;
  }

  dtlookup = new dt_entry[counts[DOCS]+1];
  dt_entry* e;
  lemur::api::TERMID_T tid =0;
  lemur::api::FILEID_T fid =0;
  long off =0;
  int len =0;
  while (fscanf(in, "%d %d %d %d", &tid, &fid, &off, &len) == 4) {
    e = &dtlookup[tid];
    e->fileid = fid;
    e->offset = off;
    e->length = len;
  }
  fclose(in);
  return true;
}

bool lemur::index::InvIndex::dtLookup() {
  *msgstream << "Trying to open dtlist lookup: " << names[TERM_LOOKUP] << endl;

  FILE* in = fopen(names[TERM_LOOKUP].c_str(), "rb");

  if (in == NULL) {
    *msgstream << "Couldn't open dt lookup table for reading" << endl;
    return false;
  }

  dtlookup = new dt_entry[counts[DOCS]+1];
  dt_entry* e;
  lemur::api::TERMID_T tid =0;
  lemur::api::FILEID_T fid =0;
  long off =0;
  int len =0;
  int mgr =0;
  while (fscanf(in, "%d %d %d %d %d", &tid, &fid, &off, &len, &mgr) == 5) {
    e = &dtlookup[tid];
    e->fileid = fid;
    e->offset = off;
    e->length = len;
    e->docmgr = mgr;
  }
  fclose(in);
  return true;
}

bool lemur::index::InvIndex::dtFileIDs() {
  ifstream in (names[TERM_INDEX].c_str());
  *msgstream << "Trying to open term index filenames: " << names[TERM_INDEX] << endl;
  if (!in.is_open()) {
    *msgstream << "Error opening term index filenames file" << endl;
    return false;
  }

  dtfiles = new string [counts[DT_FILES]];
  dtfstreams = new ifstream[counts[DT_FILES]];

  std::string line;
  int index;

  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // second number is ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      index = atoi(num1.c_str());
      value = value.substr(space1 + 1);
      dtfiles[index] = value;
      dtfstreams[index].open(dtfiles[index].c_str(), ios::in | ios::binary);
      if (dtfstreams[index] == NULL)
        cerr << "error opening: " << dtfiles[index] << endl;
    }
  }
  in.close();
  return true;
}

bool lemur::index::InvIndex::docMgrIDs() {
  ifstream in (names[DOCMGR_IDS].c_str());
  *msgstream << "Trying to open doc manager ids file: " << names[DOCMGR_IDS] << endl;
  if (!in.is_open()) {
    *msgstream << "Error opening doc manager ids" << std::endl;
    return false;
  }

  std::string line;
  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      //  numbers are ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      value = value.substr(space1 + 1);
      lemur::api::DocumentManager* dm = lemur::api::DocMgrManager::openDocMgr(value.c_str());
      docmgrs.push_back(dm);
    }
  }
  in.close();
  return true;
}

bool lemur::index::InvIndex::invFileIDs() {
  ifstream in (names[DOC_INDEX].c_str());
  *msgstream << "Trying to open inverted index filenames: " << names[DOC_INDEX] << endl;
  if (!in.is_open()) {
    *msgstream <<  "Error opening inverted index filenames file"<< endl;
    return false;
  }

  invfiles = new string[counts[INV_FILES]];
  invfstreams = new ifstream[counts[INV_FILES]];

  std::string line;
  int index;

  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // second number is ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      index = atoi(num1.c_str());
      value = value.substr(space1 + 1);
      invfiles[index] = value;
      invfstreams[index].open(invfiles[index].c_str(), ios::in | ios::binary);
    }
  }
  in.close();
  return true;
}

bool lemur::index::InvIndex::termIDs() {
  ifstream in (names[TERM_IDS].c_str());
  *msgstream << "Trying to open term ids file: " << names[TERM_IDS] << endl;
  if (in == NULL) {
    *msgstream <<  "Error opening termfile" << endl;
    return false;
  }
  terms = new lemur::api::TERM_T[counts[UNIQUE_TERMS]+1];
  std::string line;
  int index;

  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // second number is ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      index = atoi(num1.c_str());
      value = value.substr(space1 + 1);
      terms[index] = value;
      termtable[terms[index]] = index;
    }
  }
  in.close();
  if (termtable.size() != counts[UNIQUE_TERMS]+1) {
    *msgstream << "Warning:Didn't read in as many terms as we were expecting." << endl;
    *msgstream << "could be corrupted file or duplicate terms in file" << endl;
  }
  return true;
}

bool lemur::index::InvIndex::docIDs() {
  ifstream in (names[DOC_IDS].c_str());
  *msgstream<< "Trying to open doc ids file: " << names[DOC_IDS] << endl;
  if (in == NULL) {
    *msgstream << "Error opening docfile" << endl;
    return false;
  }
  docnames = new lemur::api::EXDOCID_T[counts[DOCS]+1];
  std::string line;
  int index;

  while (getline(in, line)) {
    std::string::size_type space = line.find(" ",0);
    // if the line does not have a space, something is bad here.
    if (space !=  std::string::npos) {
      // second number is ignored.
      std::string num1 = line.substr(0, space);
      std::string value = line.substr(space + 1);
      std::string::size_type space1 = value.find(" ",0);
      std::string num2 = value.substr(0, space1);
      index = atoi(num1.c_str());
      value = value.substr(space1 + 1);
      docnames[index] = value;
      doctable[docnames[index]] = index;
    }
  }
  in.close();
  if (doctable.size() != counts[DOCS]+1) {
    *msgstream << "Warning:Didn't read in as many docids as we were expecting" << endl;
    *msgstream << "could be corrupted file or duplicate docids in file" << endl;
  }
  return true;
}

void lemur::index::InvIndex::setMesgStream(ostream * lemStream) {
  msgstream = lemStream;
}
