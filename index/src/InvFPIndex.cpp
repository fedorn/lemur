#include "InvFPIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * tnt 04/2001 - created
 *
 *========================================================================*/
InvFPIndex::InvFPIndex() {
  lookup = NULL;  
}

InvFPIndex::InvFPIndex(char* indexName) {
  open(indexName);
}

InvFPIndex::~InvFPIndex() {
  if (lookup != NULL)
    delete[](lookup);
}

bool InvFPIndex::open(const char* indexName){
  counts = new int[3];
  counts[TOTAL_TERMS] = 315;
  counts[UNIQUE_TERMS] = 174;
  counts[DOCS] = 23;
  names = new char*[6];
  names[DOC_INDEX] = strcat((char*)indexName, INVINDEX);
  names[DOC_LOOKUP] = strcat((char*)indexName, INVLOOKUP);
  names[TERM_INDEX] = strcat((char*)indexName, DTINDEX);
  names[TERM_LOOKUP] = strcat((char*)indexName, DTLOOKUP);
  names[TERM_IDS] = strcat((char*)indexName, TERMIDMAP);
  names[DOC_IDS] = strcat((char*)indexName, DOCIDMAP);

  if (!indexLookup())
    return false;

  if (!dtLookup())
    return false;

  if (!termIDs())
    return false;

  if (!docIDs())
    return false;

  return true;
}

bool InvFPIndex::openFile(char* indexName){
  return false;
}

int InvFPIndex::term(const char* word){
  map<TERM_T, TERMID_T, ltstr>::iterator point = termtable.find((char*)word);
  if (point != termtable.end()) 
    return point->second;

  return -1;
}

char* InvFPIndex::term(int termID) {
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS]))
    return NULL;
  return terms[termID];
}

int InvFPIndex::document(const char* docIDStr){
  map<EXDOCID_T, DOCID_T, ltstr>::iterator point = doctable.find( (char*) docIDStr);
  if (point != doctable.end()) 
    return point->second;

  return -1;
}

char* InvFPIndex::document(int docID) { 
  if ((docID < 0) || (docID > counts[DOCS]))
    return NULL;
  return docnames[docID]; 
}

int InvFPIndex::termCount(int termID) const{
  // the termCount is equal to the position counts
  // this is calculated by the length of the inverted list -(df*2) for each doc, 
  // there is the docid and the tf
  TERM_T t;
  ifstream look;
  look.open(names[DOC_INDEX], ios::in | ios::binary);
  if (!look) {
    fprintf(stderr, "Could not open indexfile for reading.\n");
    return -1;
  }

  look.seekg(lookup[termID].offset, ios::beg);
  // we know the first thing is TERMID and we don't need it
  look.read((char*)&t, sizeof(TERMID_T));
  // now read in what we really want
  int* stuff = new int[3];
  look.read((char*)&stuff, sizeof(int)*3);
  look.close();
  if ( !(look.gcount() == sizeof(int)*3) ) {
    delete[](stuff);
    return -1;
  }
  int length = stuff[2];
  int docf = stuff[0];

  delete[](stuff);
  return length - (docf*2); 
}

float InvFPIndex::docLengthAvg(){
  return 0;
}

int InvFPIndex::docCount(int termID) {
  int df;
  TERMID_T t;
  ifstream look;
  look.open(names[DOC_INDEX], ios::in | ios::binary);
  if (!look) {
    fprintf(stderr, "Could not open indexfile for reading.\n");
    return -1;
  }

  look.seekg(lookup[termID].offset, ios::beg);
  // we know the first thing is TERMID and we don't need it
  look.read((char*)&t, sizeof(TERMID_T));
  // now read in what we really want
  look.read((char*)&df, sizeof(int));
  look.close();
  if ( !(look.gcount() == sizeof(int)) ) 
    return -1;
  
  return df;

/*
  int df;
  FILE* look = fopen(names[DOC_INDEX], "rb");
  if (look == NULL) {
    fprintf(stderr, "Could not open indexfile for reading.\n");
    return NULL;
  }
  
  if (fseek(look, lookup[termID].offset, SEEK_SET) == 0) {
    // read in the df
    if (fscanf(look, "%d", &df) != 1) {
      fclose(look);
      return 0;
    }
    fclose(look);
    return df;
  } else {
    fclose(look);
    return 0;
  }
  */
}

int InvFPIndex::docLengthTotal(int docID){
  int dl;
  DOCID_T id;
  ifstream look;
  look.open(names[TERM_INDEX], ios::in | ios::binary);
  if (!look) {
    fprintf(stderr, "Could not open term indexfile for reading.\n");
    return -1;
  }
  look.seekg(dtlookup[docID].offset, ios::beg);
  look.read((char*)&id, sizeof(DOCID_T));
  look.read((char*)&dl, sizeof(int));
  look.close();
  if ( !(look.gcount() == sizeof(int)) ) 
    return -1;
  
  return dl;
}

int InvFPIndex::docLength(int docID) const{
  int dl;
  DOCID_T id;
  ifstream look;
  look.open(names[TERM_INDEX], ios::in | ios::binary);
  if (!look) {
    fprintf(stderr, "Could not open term indexfile for reading.\n");
    return -1;
  }
  look.seekg(dtlookup[docID].offset, ios::beg);
  look.read((char*)&id, sizeof(DOCID_T));
  look.read((char*)&dl, sizeof(int));
  look.read((char*)&dl, sizeof(int));
  look.close();
  if ( !(look.gcount() == sizeof(int)) ) 
    return -1;
  
  return dl;
}

DocInfoList* InvFPIndex::docInfoList(int termID){
  fprintf(stderr, "looking for termid %d\n", termID);
//  entry e = contents[wordid];
//  char* filename = IDmanager.getFile(e.fileid);
// for now our index is always one file
  if ((termID < 0) || (termID >= counts[UNIQUE_TERMS]))
    return NULL;

  ifstream indexin;

  indexin.open(names[DOC_INDEX], ios::in | ios::binary);
  indexin.seekg(lookup[termID].offset, ios::beg);
  DocInfoList* doclist;
  InvFPDocList* dlist = new InvFPDocList();
  if (!dlist->binRead(indexin)) {
    indexin.close();
    return NULL;
  } else {
    indexin.close();
    doclist = dlist;
    return doclist;
  }

/*
  FILE* look = fopen(names[DOC_INDEX], "rb");
  if (look == NULL) {
    fprintf(stderr, "Could not open indexfile for reading.\n");
    return NULL;
  }
//  lookup = new entry[counts[UNIQUE_TERMS]];  

  
  
  int length = lookup[termID].length;
  if (fseek(look, lookup[termID].offset, SEEK_SET) == 0) {
    list = (TERMID_T*) malloc(sizeof(TERMID_T) * length);

    // read in the list
    for (int i=0;i<length;i++) {
      fscanf(look, "%d", &list[i]);
    }
    fclose(look);
    DocInfoList* doclist;
    InvFPDocList* dlist = new InvFPDocList();
    dlist->setList(termID, length-1, list+1, *list);
    doclist = dlist;
    return doclist;
  } else {
    fclose(look);
    return NULL;
  }*/

}

TermInfoList* InvFPIndex::termInfoList(int docID){
  if ((docID < 0) || (docID >= counts[DOCS]))
    return NULL;

  ifstream indexin;
  indexin.open(names[TERM_INDEX], ios::in | ios::binary);
  indexin.seekg(dtlookup[docID].offset, ios::beg);
  TermInfoList* termlist;
  InvFPTermList* tlist = new InvFPTermList();
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
bool InvFPIndex::indexLookup() {
  FILE* in = fopen(names[DOC_LOOKUP], "rb");
  cerr << "Trying to open invlist lookup: " << names[DOC_LOOKUP] << endl;
  if (in == NULL) {
    fprintf(stderr, "Couldn't open invlist lookup table for reading\n");
    return false;
  }

  lookup = new entry[counts[UNIQUE_TERMS]];
  entry* e;
  TERMID_T tid =0;
  int fid =0;
  long off =0;
  while (fscanf(in, "%d %d %d", &tid, &fid, &off) == 3) {
    e = &lookup[tid];
    e->fileid = fid;
    e->offset = off;
  }
  fclose(in);
 
  return true;
}

bool InvFPIndex::dtLookup() {
  FILE* in = fopen(names[TERM_LOOKUP], "rb");
  cerr << "Trying to open dtlist lookup: " << names[TERM_LOOKUP] << endl;
  if (in == NULL) {
    fprintf(stderr, "Couldn't open dt lookup table for reading\n");
    return false;
  }

  dtlookup = new entry[counts[DOCS]];
  entry* e;
  TERMID_T tid =0;
  int fid =0;
  long off =0;
  while (fscanf(in, "%d %d %d", &tid, &fid, &off) == 3) {
    e = &dtlookup[tid];
    e->fileid = fid;
    e->offset = off;
  }
  fclose(in);
  return true;
}

bool InvFPIndex::termIDs() {
  FILE* in = fopen(names[TERM_IDS], "rb");
  if (in == NULL) {
    fprintf(stderr, "Error opening termfile\n");
    return false;
  }
  terms = new TERM_T[counts[UNIQUE_TERMS]];

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
  if (termtable.size() != counts[UNIQUE_TERMS]) {
    cerr << "Didn't read in as many terms as we were expecting" << endl;
    return false;
  }

  return true;
}

bool InvFPIndex::docIDs() {
  FILE* in = fopen(names[DOC_IDS], "rb");
  if (in == NULL) {
    fprintf(stderr, "Error opening docfile\n");
    return false;
  }
  docnames = new EXDOCID_T[counts[DOCS]];

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

  if (doctable.size() != counts[DOCS]) {
    cerr << "Didn't read in as many docids as we were expecting" << endl;
    return false;
  }

  return true;
}
