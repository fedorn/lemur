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
  aveDocLen = 0;
}

InvIndex::InvIndex(const char* indexName) {
  lookup = NULL;  
  dtlookup = NULL;
  aveDocLen = 0;
  open(indexName);
}

InvIndex::~InvIndex() {
  if (lookup != NULL)
    delete[](lookup);
  if (dtlookup  != NULL)
    delete[](dtlookup);
  if (names != NULL) {
    for (int i = 0; i < 6; i++) {
      if (names[i] != NULL) {
        delete[](names[i]);
        names[i] = NULL;
      }
    }
    delete[](names);
  }
}

bool InvIndex::open(const char* indexName){
  counts = new int[5];
  names = new char*[6];  

  if (!fullToc(indexName)) {
    cerr << "Couldn't not properly parse param file. Index was not loaded." << endl;
    return false;
  }

  if (!indexLookup())
    return false;

  if (!dtLookup())
    return false;

  if (!dtFileIDs())
    return false;

  if (!invFileIDs())
    return false;

  if (!termIDs())
    return false;

  if (!docIDs())
    return false;

  cerr << "Load index complete." << endl;
  return true;
}

bool InvIndex::openName(char* indexName){
  counts = new int[5];
  names = new char*[6];  
  int nameLen = strlen(indexName)+1;

  char * tocName = new char[nameLen + strlen(MAINTOC) + 1];
  strncpy(tocName, indexName, nameLen);
  strcpy(tocName + nameLen, MAINTOC);

  if (!mainToc(tocName)) {
    delete[](tocName);
    return false;
  }
  delete[](tocName);

  names[DOC_INDEX] = new char[nameLen + strlen(INVINDEX)];
  strncpy(names[DOC_INDEX], indexName, nameLen);
  strcpy(names[DOC_INDEX] + nameLen, INVINDEX);

  names[DOC_LOOKUP] = new char[nameLen + strlen(INVLOOKUP)];
  strncpy(names[DOC_LOOKUP], indexName, nameLen);
  strcpy(names[DOC_LOOKUP] + nameLen, INVLOOKUP);

  names[TERM_INDEX] = new char[nameLen + strlen(DTINDEX)];
  strncpy(names[TERM_INDEX], indexName, nameLen);
  strcpy(names[TERM_INDEX] + nameLen, DTINDEX);

  names[TERM_LOOKUP] = new char[nameLen + strlen(DTLOOKUP)];
  strncpy(names[TERM_LOOKUP], indexName, nameLen);
  strcpy(names[TERM_LOOKUP] + nameLen, DTLOOKUP);

  names[TERM_IDS] = new char[nameLen + strlen(TERMIDMAP)];
  strncpy(names[TERM_IDS], indexName, nameLen);
  strcpy(names[TERM_IDS] + nameLen, TERMIDMAP);

  names[DOC_IDS] = new char[nameLen + strlen(DOCIDMAP)];
  strncpy(names[DOC_IDS], indexName, nameLen);
  strcpy(names[DOC_IDS] + nameLen, DOCIDMAP);

  if (!indexLookup())
    return false;

  if (!dtLookup())
    return false;

  if (!dtFileIDs())
    return false;

  if (!invFileIDs())
    return false;

  if (!termIDs())
    return false;

  if (!docIDs())
    return false;

  cerr << "Load index complete." << endl;
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

int InvIndex::termCount(int termID) const{
  // the termCount is equal to the position counts
  // this is calculated by the length of the inverted list -(df*2) for each doc, 
  // there is the docid and the tf
  if ((termID <0) || (termID > counts[UNIQUE_TERMS])) {
    cerr << "Error:  Trying to get termCount for invalid termID" << endl;
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
    cerr << "Error:  Trying to get docCount for invalid termID" << endl;
    return 0;
  }

  // this is OOV
  if (termID == 0)
    return 0;

  return lookup[termID].df;
}

int InvIndex::docLength(int docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    fprintf(stderr, "Error trying to get docLength for invalid docID.\n");
    return 0;
  }
    
  if (docID == 0)
    return 0;
  if (!dtloaded) {
    cerr << "DT index must be loaded to obtain docLength" << endl;
    return 0;
  }
  return dtlookup[docID].length;
}

int InvIndex::docLengthCounted(int docID) {
  if ((docID < 0) || (docID > counts[DOCS])) {
    fprintf(stderr, "Error trying to get docLengthCounted for invalid docID.\n");
    return 0;
  }
  if (!dtloaded) {
    cerr << "DT index must be loaded to obtain docLength" << endl;
    return 0;
  }
    
  if (docID == 0)
    return 0;

  int dl;
  DOCID_T id;
  ifstream look;
  look.open(dtfiles[dtlookup[docID].fileid], ios::in | ios::binary);
  if (!look) {
    fprintf(stderr, "Could not open term indexfile for reading.\n");
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
    cerr << "Error:  Trying to get docInfoList for invalid termID" << endl;    
    return NULL;
  }

  if (termID == 0)
    return NULL;

  ifstream indexin;

  indexin.open(invfiles[lookup[termID].fileid], ios::in | ios::binary);
  indexin.seekg(lookup[termID].offset, ios::beg);
  DocInfoList* doclist;
  InvDocList* dlist = new InvDocList();
  if (!dlist->binRead(indexin)) {
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
    fprintf(stderr, "Error trying to get termInfoList for invalid docID.\n");
    return NULL;
  }
  if (!dtloaded) {
    cerr << "DT index must be loaded to obtain termInfoList" << endl;
    return 0;
  }
    
  if (docID == 0)
    return NULL;

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
  cerr << "Trying to open toc: " << fileName << endl;
  if (in == NULL) {
    cerr << "Couldn't open toc file for reading" << endl;
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
    }
  }    

  aveDocLen = counts[TOTAL_TERMS] / (float) counts[DOCS];

  return true;
}

bool InvIndex::mainToc(char * fileName) {
  FILE* in = fopen(fileName, "r");
  cerr << "Trying to open toc: " << fileName << endl;
  if (in == NULL) {
    cerr << "Couldn't open toc file for reading" << endl;
    return false;
  }

  char key[128];
  char val[128];
  while (!feof(in)) {
    fscanf(in, "%s %s", key, val);
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
    }

  }

  aveDocLen = counts[TOTAL_TERMS] / (float) counts[DOCS];

  return true;
}

bool InvIndex::indexLookup() {
  FILE* in = fopen(names[DOC_LOOKUP], "rb");
  cerr << "Trying to open invlist lookup: " << names[DOC_LOOKUP] << endl;
  if (in == NULL) {
    fprintf(stderr, "Couldn't open invlist lookup table for reading\n");
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

bool InvIndex::dtLookup() {
  FILE* in = fopen(names[TERM_LOOKUP], "rb");
  dtloaded = ParamGetInt("loadDT", 1);
  if (!dtloaded) {
    cerr << "Skipping dtlist at user request" << endl;
    return true;
  }
  cerr << "Trying to open dtlist lookup: " << names[TERM_LOOKUP] << endl;
  if (in == NULL) {
    fprintf(stderr, "Couldn't open dt lookup table for reading\n");
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

bool InvIndex::dtFileIDs() {
  FILE* in = fopen(names[TERM_INDEX], "rb");
  cerr << "Trying to open term index filenames: " << names[TERM_INDEX] << endl;
  if (in == NULL) {
    fprintf(stderr, "Error opening term index filenames file\n");
    return false;
  }

  dtfiles = new char*[counts[DT_FILES]];

  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = (char*) malloc(sizeof(char) * (len+1));
    if (fscanf(in, "%s", str) != 1) {
       free(str);
       continue;
    }

    dtfiles[index] = str;
  }

  fclose(in);
  return true;
}

bool InvIndex::invFileIDs() {
  FILE* in = fopen(names[DOC_INDEX], "rb");
  cerr << "Trying to open inverted index filenames: " << names[DOC_INDEX] << endl;
  if (in == NULL) {
    fprintf(stderr, "Error opening inverted index filenames file\n");
    return false;
  }

  invfiles = new char*[counts[INV_FILES]];

  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = (char*) malloc(sizeof(char) * (len+1));
    if (fscanf(in, "%s", str) != 1) {
       free(str);
       continue;
    }

    invfiles[index] = str;
  }

  fclose(in);
  return true;
}

bool InvIndex::termIDs() {
  FILE* in = fopen(names[TERM_IDS], "rb");
  cerr << "Trying to open term ids file: " << names[TERM_IDS] << endl;
  if (in == NULL) {
    fprintf(stderr, "Error opening termfile\n");
    return false;
  }
  terms = new TERM_T[counts[UNIQUE_TERMS]+1];

  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = (char*) malloc(sizeof(char) * (len+1));
    if (fscanf(in, "%s", str) != 1) {
       free(str);
       continue;
    }

    terms[index] = str;
    termtable[terms[index]] = index;
  }

  fclose(in);
  if (termtable.size() != counts[UNIQUE_TERMS]+1) {
    cerr << "Didn't read in as many terms as we were expecting" << endl;
    return false;
  }

  return true;
}

bool InvIndex::docIDs() {
  FILE* in = fopen(names[DOC_IDS], "rb");
  cerr << "Trying to open doc ids file: " << names[DOC_IDS] << endl;
  if (in == NULL) {
    fprintf(stderr, "Error opening docfile\n");
    return false;
  }
  docnames = new EXDOCID_T[counts[DOCS]+1];

  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2)
      continue;

    str = (char*) malloc(sizeof(char) * (len+1));
    if (fscanf(in, "%s", str) != 1) {
      free(str);
      continue;
    }

    docnames[index] = str;
    doctable[docnames[index]] = index;
  }
  fclose(in);

  if (doctable.size() != counts[DOCS]+1) {
    cerr << "Didn't read in as many docids as we were expecting" << endl;
    return false;
  }

  return true;
}
