/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "InvIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * tnt 04/2002 - created
 *
 *========================================================================*/

InvIndex::InvIndex() {
  lookup = NULL;  
  dtlookup = NULL;
  dtfiles = NULL;
  invfiles = NULL;
  docnames = NULL;
  terms = NULL;
  aveDocLen = 0;
}

InvIndex::InvIndex(const char* indexName) {
  lookup = NULL;  
  dtlookup = NULL;
  dtfiles = NULL;
  invfiles = NULL;
  docnames = NULL;
  terms = NULL;
  aveDocLen = 0;
  open(indexName);
}

InvIndex::~InvIndex() {
  int i;
  if (lookup != NULL)
    delete[](lookup);
  if (dtlookup  != NULL)
    delete[](dtlookup);
  if (names != NULL) {
    for (i = 0; i < NAMES_SIZE; i++) {
      if (names[i] != NULL) {
	free(names[i]); //	delete[](names[i]); // allocated w strdup
        names[i] = NULL;
      }
    }
    delete[](names);
  }
  
  if (dtfiles != NULL) {
    for (i=0;i<counts[DT_FILES];i++) {
      delete[](dtfiles[i]);
    }
    delete[](dtfiles);
  }

  if (invfiles != NULL) {
    for (i=0;i<counts[INV_FILES];i++) {
      delete[](invfiles[i]);
    }
    delete[](invfiles);
  }

  for (i = 0; i < docmgrs.size(); i++)
    delete[](docmgrs[i]);

  if (terms != NULL) {
    for (i = 0; i < counts[UNIQUE_TERMS]+1; i++)
      delete[](terms[i]);
    delete[](terms);
  }

  if (docnames != NULL) {
    for (i = 0; i < counts[DOCS]+1; i++)
      delete[](docnames[i]);
    delete[](docnames);
  }

  delete[](counts);
}

bool InvIndex::open(const char* indexName){
  counts = new int[5];
  names = new char*[NAMES_SIZE];  
  names[VERSION_NUM] = "";
  names[DOCMGR_IDS] = NULL;

  String streamSelect = ParamGetString("stream", "cerr");
  if (streamSelect == "cout") {
    setMesgStream(&cout);
  } else {
    setMesgStream(&cerr);
  }

  if (!fullToc(indexName)) {
    cerr << "Couldn't not properly parse param file. Index was not loaded." << endl;
    return false;
  }

  dtloaded = ParamGetInt("loadDT", 1);

  if (!dtloaded) {
    *msgstream << "Parameter set to dtlist lookup not being loaded" << endl;
  } else {

    // older versions didn't specify version name and will be blank
    // if it's new we call new load functions
    if (strcmp(names[VERSION_NUM], "")) {
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

int InvIndex::term(const char* word){
  map<TERM_T, TERMID_T, ltstr>::iterator point = termtable.find((char*)word);
  if (point != termtable.end()) 
    return point->second;

  return 0;
}

const char* InvIndex::term(int termID) {
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS]))
    return NULL;
  return terms[termID];
}

int InvIndex::document(const char* docIDStr){
  map<EXDOCID_T, DOCID_T, ltstr>::iterator point = doctable.find( (char*) docIDStr);
  if (point != doctable.end()) 
    return point->second;

  return 0;
}

const char* InvIndex::document(int docID) { 
  if ((docID < 0) || (docID > counts[DOCS]))
    return NULL;
  return docnames[docID]; 
}

const char* InvIndex::docManager(int docID) {
  // no such thing previous to version 1.9
  if (!strcmp(names[VERSION_NUM], "")) 
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

int InvIndex::termCount(int termID) const{
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

float InvIndex::docLengthAvg(){
  return aveDocLen;
}

int InvIndex::docCount(int termID) {
  if ((termID <0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docCount for invalid termID" << endl;
    return 0;
  }

  // this is OOV
  if (termID == 0)
    return 0;

  return lookup[termID].df;
}

int InvIndex::docLength(int docID) const{
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

int InvIndex::docLengthCounted(int docID) {
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

  int dl;
  DOCID_T id;
  ifstream look;
  look.open(dtfiles[dtlookup[docID].fileid], ios::in | ios::binary);
  if (!look) {
    *msgstream << "Could not open term indexfile for reading." << endl;
    return 0;
  }
  look.seekg(dtlookup[docID].offset, ios::beg);
  look.read((char*)&id, sizeof(DOCID_T));
  look.read((char*)&dl, sizeof(int));
  look.read((char*)&dl, sizeof(int));
  look.close();
  if ( !(look.gcount() == sizeof(int)) ) 
    return 0;
  
  return dl;
}

DocInfoList* InvIndex::docInfoList(int termID){
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docInfoList for invalid termID" << endl;
    return NULL;
  }

  if (termID == 0)
    return NULL;

  ifstream indexin;

  indexin.open(invfiles[lookup[termID].fileid], ios::in | ios::binary);
  indexin.seekg(lookup[termID].offset, ios::beg);
  DocInfoList* doclist;
  InvDocList* dlist = new InvDocList();
  bool success;

  if (strcmp(names[VERSION_NUM], "")) {
    // version 1.9 is compressed and must be decompressed
    success = dlist->binReadC(indexin);
  } else {
    success = dlist->binRead(indexin);
  }
  if (!success) {
    indexin.close();
    return NULL;
  } else {
    indexin.close();
    doclist = dlist;
    return doclist;
  }
}

TermInfoList* InvIndex::termInfoList(int docID){
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

  ifstream indexin;
  indexin.open(dtfiles[dtlookup[docID].fileid], ios::in | ios::binary);
  indexin.seekg(dtlookup[docID].offset, ios::beg);
  TermInfoList* termlist;
  InvTermList* tlist = new InvTermList();
  if (!tlist->binRead(indexin)) {
    indexin.close();
    return NULL;
  } 

  indexin.close();
  termlist = tlist;
  return termlist;
}

/*=======================================================================
 * PRIVATE METHODS 
 =======================================================================*/
bool InvIndex::fullToc(const char * fileName) {
  FILE* in = fopen(fileName, "rb");
  *msgstream << "Trying to open toc: " << fileName << endl;
  if (in == NULL) {
     *msgstream << "Couldn't open toc file for reading" << endl;
    return false;
  }

  char key[128];
  char val[128];
  while (!feof(in)) {
    fscanf(in, "%s %s", key, val);
    cerr << key << ":" << val << endl;
    if (strcmp(key, NUMDOCS_PAR) == 0) {
      counts[DOCS] = atoi(val);
    } else if (strcmp(key, NUMTERMS_PAR) == 0) {
      counts[TOTAL_TERMS] = atoi(val);
    } else if (strcmp(key, NUMUTERMS_PAR) == 0) {
      counts[UNIQUE_TERMS] = atoi(val);
    } else if (strcmp(key, NUMDT_PAR) == 0) {
      counts[DT_FILES] = atoi(val);
    } else if (strcmp(key, NUMINV_PAR) == 0) {
      counts[INV_FILES] = atoi(val);
    } else if (strcmp(key, INVINDEX_PAR) == 0) {
      names[DOC_INDEX] = strdup(val);
    } else if (strcmp(key, INVLOOKUP_PAR) == 0) {
      names[DOC_LOOKUP] = strdup(val);
    } else if (strcmp(key, DTINDEX_PAR) == 0) {
      names[TERM_INDEX] = strdup(val);
    } else if (strcmp(key, DTLOOKUP_PAR) == 0) {
      names[TERM_LOOKUP] = strdup(val);
    } else if (strcmp(key, TERMIDMAP_PAR) == 0) {
      names[TERM_IDS] = strdup(val);
    } else if (strcmp(key, DOCIDMAP_PAR) == 0) {
      names[DOC_IDS] = strdup(val);
    } else if (strcmp(key, DOCMGR_PAR) == 0) {
      names[DOCMGR_IDS] = strdup(val);
    } else if (strcmp(key, VERSION_PAR) == 0) {
      names[VERSION_NUM] = strdup(val);
    }
  }    

  aveDocLen = counts[TOTAL_TERMS] / (float) counts[DOCS];

  return true;
}

bool InvIndex::indexLookup() {
  FILE* in = fopen(names[DOC_LOOKUP], "rb");
  *msgstream << "Trying to open invlist lookup: " << names[DOC_LOOKUP] << endl;
  if (in == NULL) {
    *msgstream << "Couldn't open invlist lookup table for reading" << endl;
    return false;
  }

  lookup = new inv_entry[counts[UNIQUE_TERMS]+1];
  inv_entry* e;
  TERMID_T tid =0;
  int fid =0;
  long off =0;
  int ctf = 0;
  int df = 0;
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

bool InvIndex::dtLookup_ver1() {
  FILE* in = fopen(names[TERM_LOOKUP], "rb");
  *msgstream << "Trying to open dtlist lookup: " << names[TERM_LOOKUP] << endl;
  if (in == NULL) {
    *msgstream <<  "Couldn't open dt lookup table for reading" << endl;
    return false;
  }

  dtlookup = new dt_entry[counts[DOCS]+1];
  dt_entry* e;
  TERMID_T tid =0;
  int fid =0;
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

bool InvIndex::dtLookup() {
  *msgstream << "Trying to open dtlist lookup: " << names[TERM_LOOKUP] << endl;

  FILE* in = fopen(names[TERM_LOOKUP], "rb");

  if (in == NULL) {
    *msgstream << "Couldn't open dt lookup table for reading" << endl;
    return false;
  }

  dtlookup = new dt_entry[counts[DOCS]+1];
  dt_entry* e;
  TERMID_T tid =0;
  int fid =0;
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

bool InvIndex::dtFileIDs() {
  FILE* in = fopen(names[TERM_INDEX], "rb");
  *msgstream << "Trying to open term index filenames: " << names[TERM_INDEX] << endl;
  if (in == NULL) {
    *msgstream << "Error opening term index filenames file" << endl;
    return false;
  }


  dtfiles = new char*[counts[DT_FILES]];

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
    str[len] = '\0';
    dtfiles[index] = str;
  }

  fclose(in);
  return true;
}

bool InvIndex::docMgrIDs() {
  FILE* in = fopen(names[DOCMGR_IDS], "r");
  *msgstream << "Trying to open doc manager ids file: " << names[DOCMGR_IDS] << endl;
  if (in == NULL) {
    fprintf(stderr, "Error opening doc manager ids \n");
    return false;
  }

  int ind, len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &ind, &len) != 2)
      continue;

    str = new char[len + 1]; 
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    str[len] = '\0';
    docmgrs.push_back(str);
  }
  fclose(in);
  return true;
}

bool InvIndex::invFileIDs() {
  FILE* in = fopen(names[DOC_INDEX], "rb");
  *msgstream << "Trying to open inverted index filenames: " << names[DOC_INDEX] << endl;
  if (in == NULL) {
    *msgstream <<  "Error opening inverted index filenames file"<< endl;
    return false;
  }

  invfiles = new char*[counts[INV_FILES]];

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
    str[len] = '\0';
    invfiles[index] = str;
  }

  fclose(in);
  return true;
}

bool InvIndex::termIDs() {
  FILE* in = fopen(names[TERM_IDS], "rb");
  *msgstream << "Trying to open term ids file: " << names[TERM_IDS] << endl;
  if (in == NULL) {
    *msgstream <<  "Error opening termfile" << endl;
    return false;
  }
  terms = new TERM_T[counts[UNIQUE_TERMS]+1];

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
    str[len] = '\0';
    terms[index] = str;
    termtable[terms[index]] = index;
  }

  fclose(in);
  if (termtable.size() != counts[UNIQUE_TERMS]+1) {
    *msgstream << "Warning:Didn't read in as many terms as we were expecting." << endl;
    *msgstream << "could be corrupted file or duplicate terms in file" << endl;
  }

  return true;
}

bool InvIndex::docIDs() {
  FILE* in = fopen(names[DOC_IDS], "rb");
   *msgstream<< "Trying to open doc ids file: " << names[DOC_IDS] << endl;
  if (in == NULL) {
    *msgstream << "Error opening docfile" << endl;
    return false;
  }
  docnames = new EXDOCID_T[counts[DOCS]+1];

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
    str[len] = '\0';
    docnames[index] = str;
    doctable[docnames[index]] = index;
  }
  fclose(in);

  if (doctable.size() != counts[DOCS]+1) {
    *msgstream << "Warning:Didn't read in as many docids as we were expecting" << endl;
    *msgstream << "could be corrupted file or duplicate docids in file" << endl;
  }
  return true;
}

void InvIndex::setMesgStream(ostream * lemStream) {
  msgstream = lemStream;
}
