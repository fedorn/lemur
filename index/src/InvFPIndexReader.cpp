#include "InvFPIndexReader.hpp"

InvFPIndexReader::InvFPIndexReader() {
  reader = NULL;
  list = NULL;
  current = new InvFPDocList();
}

InvFPIndexReader::InvFPIndexReader(string fname) {
	//	fprintf(stderr, "opening %s to read", fname.c_str());
  //fprintf(stderr, "string\n");
	reader = fopen(fname.c_str(), "rb");
  current = new InvFPDocList();
  list = NULL;
}

InvFPIndexReader::InvFPIndexReader(char* fname) {
	//	fprintf(stderr, "opening %s to read", fname.c_str());
  //fprintf(stderr, "char*\n");
	reader = fopen(fname, "rb");
  current = new InvFPDocList();
  list = NULL;
}

InvFPIndexReader::~InvFPIndexReader() {
  delete(current);
  fclose(reader);
  if (list != NULL)
    free(list);
}

void InvFPIndexReader::setFStream(FILE* fstream) {
  reader = fstream;
}

void InvFPIndexReader::setFStream(string fname) {
  reader = fopen(fname.c_str(), "rb");
}

bool InvFPIndexReader::EOIndex() {
  if (feof(reader) == 0)
    return false;
  return true;
}

/// returns NULL if cannot read next word and list due to error
/// we are expecting a list of this format:
/// termid, size of list, df, docid1, tf, pos1, ..pos_tf, .. docid_df, tf, pos1, ..pos_tf
InvFPDocList* InvFPIndexReader::readNextList() {
  //	fprintf(stderr, "reading word\n");
  //	free(word);
  if (list != NULL) {
    free(list);
    list = NULL;
  }

  //llength = 0;
	
	int llength;
  TERMID_T termid;

  /*
    if (fread(&wlength, sizeof(int), 1, reader) != 1) 
    return NULL;
    word = (char*) malloc(wlength * sizeof(char));
    if (fread(word, sizeof(char), length, reader) != length)
    return NULL;
    if (fread(&llength, sizeof(int), 1, reader) != 1)
    return NULL;
    list = (int*) malloc(llength * sizeof(int));
    if (fread(list, sizeof(int), length, reader) != length)
    return NULL;
    */
  if (fscanf(reader, "%d", &termid) != 1) {
    return NULL;
  }
  // greater than max word length, truncate word
  /*if (wlength > 1024) {
    wlength = 1024;
  } 
  word = (char*) malloc(wlength * sizeof(char));
  if (fscanf(reader, "%s", word) != 1) {
    free(word);
    wlength = 0;
    return false;
  } */

  if (fscanf(reader, "%d", &llength) != 1) {
    return NULL;
  }

  list = (int*) malloc(llength*sizeof(int));
  if (fscanf(reader, "%d", &list[0]) != 1)
    return NULL;

  int ind = 1;
  int j, tf;
  for (int i=0;i<(*list-1);i++) {
    if (fscanf(reader, "%d", &list[ind]) != 1)
      return NULL;
    ind++;
    if (fscanf(reader, "%d", &list[ind]) != 1)
      return NULL;
    tf = list[ind];
    ind++;
    for (j=0;j<tf;j++) {
      if (fscanf(reader, "%d", &list[ind]) != 1)
        return NULL;
      ind++;
    }
  }


  // now we know we're at the lastdocument
  // save the pointer and read in the rest of the list
  int* lastid = &list[ind];

  for (j=ind;j<llength;j++) {
    if (fscanf(reader, "%d", &list[j]) != 1)
      return NULL;
  }
  /*
  for (int i=0;i<llength;i++) {
    if (fscanf(reader, "%d", &list[i]) != 1) {
      return NULL;
    }
  }*/

  current->setList(termid, llength-1, list+1, *list, lastid, 0);
  return current;
}

InvFPDocList* InvFPIndexReader::currentList() {
  return current;
}