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
#include "InvIndexMerge.hpp"

InvIndexMerge::InvIndexMerge(char* buffer, long size, long maxfilesize) {
  maxfile = maxfilesize;
  readbuffer = NULL;
  setBuffer(buffer, size);
}

InvIndexMerge::InvIndexMerge(long buffersize, long maxfilesize) {
  maxfile = maxfilesize;
  readbuffer = (char*) malloc(buffersize);
  if (readbuffer)
    bufsize = buffersize;
  else {
    bufsize = 0;
    fprintf(stderr, "InvIndexMerge: Error allocating buffer for IndexMerge\n");
  }
}


InvIndexMerge::~InvIndexMerge() {
  free(name);
}

int InvIndexMerge::merge(vector<char*>* tf, char* prefix) {
  if (bufsize < READBUFSIZE * 2) {
    fprintf(stderr, "merge:  Need a larger buffer size (at least %d).\n", READBUFSIZE*2);
    return 0;
  }
  name = strdup(prefix);
  return this->hierMerge(tf, 0);
}

char* InvIndexMerge::setBuffer(char* buf, long size) {
  char* ret = NULL;

  if (readbuffer != NULL)
    ret = readbuffer;
  readbuffer = buf;  
  bufsize = size;

  return ret;
}

void InvIndexMerge::setMaxFileSize(long size) {
  if (size > 0) 
    maxfile = size;
}

int InvIndexMerge::hierMerge(vector<char*>* files, int level) {  
  fprintf(stderr, "%s: Begin hierchical merge level %d\n", name, level);
  int numfh = bufsize/READBUFSIZE;

  if (numfh > NUM_FH_OPEN) {
    numfh = NUM_FH_OPEN;
  }

  if (files->size() > numfh) {
    vector<char*> intmedfiles;
    vector<char*> subset;
    vector<char*>::iterator begin;
    vector<char*>::iterator end;

    for (begin=files->begin(); begin<files->end(); begin+=numfh) {
      if (begin+numfh < files->end())
	end = begin+numfh;
      else end = files->end();
      subset.assign(begin, end);
      mergeFiles(&subset, &intmedfiles, level);
    }
    level++;
    int num = hierMerge(&intmedfiles, level);
    return num;

  } else {
    return finalMerge(files);
  }
}

int InvIndexMerge::mergeFiles(vector<char*>* files, vector<char*>* intmed, int level) {
  fprintf(stderr, "%s: Merging Intermediate files\n", name);

  vector<IndexReader*> readers;
  char* bufptr = readbuffer;

  int namelen = strlen(name)+1;
  char* indexname = (char*)malloc(namelen+3);

  sprintf(indexname, "%s%d.%d", name, level, intmed->size());
  intmed->push_back(indexname);

  // if we're only merging one file, no merging is necessary
  if (files->size() == 1) {
    // have to do the remove because rename wont' work if file exists
    remove(indexname);
    rename((*files)[0], indexname);
    return 1;
  }
    
  for (int i=0;i<files->size();i++) {

    readers.push_back(new IndexReader);

    readers[i]->reader = new ifstream();
    setbuf(readers[i]->reader, bufptr, READBUFSIZE);
    bufptr += READBUFSIZE;

    fprintf(stderr, "Opening %s\n", (*files)[i]);
    readers[i]->reader->open((*files)[i], ios::in | ios::binary);
    if (!readers[i]->reader->is_open()) {
      fprintf(stderr, "mergeFiles %d: Error! Couldn't open %s!\n", level,(*files)[i]);
      return 0;
    }
    readers[i]->list = new InvDocList();
    if (!readers[i]->list->binReadC(*(readers[i]->reader))) 
      fprintf(stderr, "Error reading from file\n");
  }

  ofstream indexfile;
  indexfile.open(indexname, ios::binary | ios::out);

  vector<int> working;  // list of least words
  int offset;
  long filelen;
  InvDocList* list, *first;
  vector<int>::iterator iter;

  while (readers.size() > 1) {
    offset=0;
    working.clear();
    least(&readers, &working);

    iter=working.begin();
    first = readers[*iter]->list;
    for (iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      first->append(list);
    }

    // figure out if we need a new index file or if there's still room
    indexfile.flush();
    filelen = (long)indexfile.tellp();
    if (filelen + ((first->length()+4) *sizeof(int)) > maxfile) {
      indexfile.close();
      char* newindex = (char*)malloc(namelen+3);
      sprintf(newindex, "%s%d.%d", name, level, intmed->size());
      intmed->push_back(newindex);
      indexfile.open(newindex, ios::binary | ios::out);
      if (!indexfile) {
        cerr << name << " :Error in merge. Could not open another index file for writing." << endl;
        return 0;
      }
      filelen = 0;
    }

    // write out the list
    first->binWriteC(indexfile);

    //update this file reader or erase it from available readers
    IndexReader* ir;
    for (iter=working.begin(); iter!=working.end(); iter++) {
      ir = readers[*iter-offset];
      ir->list->resetFree();
      if ( !ir->list->binReadC(*(ir->reader)) ) {
        delete(ir->list);
        delete(ir->reader);
        delete(ir);
        readers.erase(readers.begin()+(*iter-offset));
	      //update the other indices
	      offset++;
      }
    }

  }

  //grab the last file open if any
  if (readers.size() != 0) {
    IndexReader* myreader = readers.front();
    // dump it out

    // check for file size
    indexfile.flush();
    filelen = (long)indexfile.tellp();
    if (filelen + ((myreader->list->length()+4) *sizeof(int)) > maxfile) {
      indexfile.close();
      char* newindex = (char*)malloc(namelen+3);
      sprintf(newindex, "%s%d.%d", name, level, intmed->size());
      intmed->push_back(newindex);
      indexfile.open(newindex, ios::binary | ios::out);
      filelen =0;
    }

    // write the current list
    myreader->list->binWriteC(indexfile);
    myreader->list->resetFree();

    //write the rest 
    while (myreader->list->binReadC(*(myreader->reader))) {
      indexfile.flush();
      filelen = (long)indexfile.tellp();
      if (filelen + ((myreader->list->length()+4) *sizeof(int)) > maxfile) {
        indexfile.close();
        char* newindex = (char*)malloc(namelen+3);
        sprintf(newindex, "%s%d.%d", name, level, intmed->size());
        intmed->push_back(newindex);
        indexfile.open(newindex, ios::binary | ios::out);
        filelen=0;
      }
      
      myreader->list->binWriteC(indexfile);
      myreader->list->resetFree();
    } //while that file

    myreader->reader->close();
    delete(myreader->list);
    delete(myreader->reader);
    delete(myreader);
	
    readers.clear();
  
  } // if still a file

  indexfile.close();

  //remove files already merged
  for (int f=0;f<files->size();f++) {
    remove((*files)[f]);
  }

  return intmed->size();
}

int InvIndexMerge::finalMerge(vector<char*>* files) {
  fprintf(stderr, "%s: Final Merge of files\n", name);
  int i=0;
  vector<IndexReader*> readers;
  char* bufptr = readbuffer;

  for (i=0;i<files->size();i++) {
    readers.push_back(new IndexReader);
    readers[i]->reader = new ifstream();
    setbuf(readers[i]->reader, bufptr, READBUFSIZE);
    bufptr += READBUFSIZE;
    fprintf(stderr, "Opening file %s\n", (*files)[i]);
    readers[i]->reader->open((*files)[i], ios::in | ios::binary);
    if (!readers[i]->reader->is_open())
      fprintf(stderr, "Error: Could not open file\n");
    readers[i]->list = new InvDocList();
    readers[i]->list->binReadC(*(readers[i]->reader));
  }

  vector<int> working;  // list of least words
  int namelen = strlen(name)+1;

  namelen += strlen(INVINDEX);
  char* indexname = (char*)malloc(namelen+1);
  sprintf(indexname, "%s%s%d", name, INVINDEX, 0);
  invfiles.push_back(indexname);
  char* lookup = (char*)malloc(namelen+strlen(INVLOOKUP));
  sprintf(lookup, "%s%s", name, INVLOOKUP);

  ofstream indexfile;
  indexfile.open(indexname, ios::binary | ios::out);
  FILE* listing = fopen(lookup, "wb");

  int fid;
  int offset, ll, df;
  long filelen;
  vector<int>::iterator iter;
  InvDocList* list, *first;

  while (readers.size() > 1) {
    offset=0;
    working.clear();
    least(&readers, &working);

    // make sure the ftell is accurate
    indexfile.flush();

    iter=working.begin();
    first = readers[*iter]->list;

    // make all the lists into one
    for (iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      first->append(list);
    }
    
    ll = first->length();
    df = first->docFreq();
    
    // figure out if we need a new index file or if there's still room
    // the length of what we write out is the length of the inverted list
    // plus the tid, df, diff, len
    filelen = (long)indexfile.tellp();
    if ((filelen + (ll+4) *sizeof(int)) > maxfile) {
      indexfile.close();
      char* newindex = (char*)malloc(namelen+1);
      sprintf(newindex, "%s%s%d", name, INVINDEX, invfiles.size());
      invfiles.push_back(newindex);
      indexfile.open(newindex, ios::binary | ios::out);
      if (!indexfile) {
        cerr << name << " :Error in merge. Could not open another index file for writing." << endl;
        return 0;
      }
      filelen = 0;
    }
    fid = invfiles.size()-1;

    // write out the lookup first
    // tid, fid, file offset, ctf, df
    fprintf(listing, "%d %d %d %d %d ", first->termID(), fid, filelen, ll-(df*2), df);

    // write out the list
    first->binWriteC(indexfile);

    //update this file reader or erase it from available readers
    IndexReader* ir;
    for (iter=working.begin(); iter!=working.end(); iter++) {
      ir = readers[*iter-offset];
      ir->list->resetFree();
      if ( !ir->list->binReadC(*(ir->reader)) ) {
        delete(ir->list);
        delete(ir->reader);
        delete(ir);
        readers.erase(readers.begin()+(*iter-offset));
	      //update the other indices
	      offset++;
      }
    }

  }

  //grab the last file open if any
  if (readers.size() != 0) {
    IndexReader* myreader = readers.front();
    // dump it out

    ll = myreader->list->length();
    df = myreader->list->docFreq();

    indexfile.flush();
    filelen = (long)indexfile.tellp();
    if (filelen + ((ll+4) *sizeof(int)) > maxfile) {
      indexfile.close();
      char* newindex = (char*)malloc(namelen+1);
      sprintf(newindex, "%s%s%d", name, INVINDEX, invfiles.size());
      invfiles.push_back(newindex);
      indexfile.open(newindex, ios::binary | ios::out);
      filelen =0;
    }
    fid = invfiles.size()-1;

    fprintf(listing, "%d %d %d %d %d ", myreader->list->termID(), fid, filelen, ll-(df*2), df);
    
    // write out the list
    myreader->list->binWriteC(indexfile);
    myreader->list->resetFree();

    //write the rest 
    while (myreader->list->binReadC(*(myreader->reader)) ){
      ll = myreader->list->length();
      df = myreader->list->docFreq();

      indexfile.flush();
      filelen = (long)indexfile.tellp();
      if (filelen + ((ll+4) *sizeof(int)) > maxfile) {
        indexfile.close();
        char* newindex = (char*)malloc(namelen+1);
        sprintf(newindex, "%s%s%d", name, INVINDEX, invfiles.size());
        invfiles.push_back(newindex);
        indexfile.open(newindex, ios::binary | ios::out);
        filelen=0;
      }
      fid = invfiles.size()-1;

      fprintf(listing, "%d %d %d %d %d ", myreader->list->termID(), fid, filelen, ll-(df*2), df);

      myreader->list->binWriteC(indexfile);
      myreader->list->resetFree();
    } //while that file

    myreader->reader->close();
    delete(myreader->list);
    delete(myreader->reader);
    delete(myreader);
    readers.clear();
  
  } // if still a file
  indexfile.close();
  fclose(listing);

  writeInvFIDs();

  free(indexname);
  free(lookup);

  //remove files already merged
  for (int f=0;f<files->size();f++) {
    remove((*files)[f]);
  }

  return invfiles.size();
}

/*=====================PRIVATE METHODS =========================*/
void InvIndexMerge::writeInvFIDs() {
  char* fidmap = (char*)malloc(strlen(name)+strlen(INVINDEX)+1);
  sprintf(fidmap, "%s%s", name, INVINDEX);
  FILE* write = fopen(fidmap, "wb");
  if (!write) {
    cerr << "Error: Couldn't create inverted index files to file ids map" << endl;
    return;
  }
  for (int i=0;i<invfiles.size();i++) {
    fprintf(write, "%d %d %s ", i, strlen(invfiles[i]), invfiles[i]);
  }
  fclose(write);
  free(fidmap);
}

void InvIndexMerge::least(vector<IndexReader*>* r, vector<int>* ret) {
  /*
  int cmp;
  char* least = readers[0]->getWord();
  ret->push_back(0);

  for (int i=1;i<readers.size(); i++) {
    cmp = strcmp(readers[i]->getWord(), least);
    if (cmp == 0) 
      //they are equal
      ret->push_back(i);
    else if (cmp < 0) {
      least = readers[i]->getWord();
      ret->clear();
      ret->push_back(i);
    }
  } */

  InvDocList* list;

  int lid, id;
  list = (*r)[0]->list;
  lid = list->termID();
  ret->push_back(0);

  for (int i=1;i<r->size(); i++) {
    list = (*r)[i]->list;
    if (list) {
      id = list->termID();
      if (id == lid) {
        ret->push_back(i);
      } else if (id < lid) {
        lid = id;
        ret->clear();
        ret->push_back(i);
      }
    } // if not NULL
  } // for each list
}	


void InvIndexMerge::setbuf(ifstream* fs, char* bp, int bytes){
  fs->rdbuf()->pubsetbuf(bp, bytes);
}