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
    if (!readers[i]->list->binRead(*(readers[i]->reader))) 
      fprintf(stderr, "Error reading from file\n");
  }

  ofstream indexfile;
  indexfile.open(indexname, ios::binary | ios::out);

  vector<int> working;  // list of least words
  map<DOCID_T, int> tfs; // combined tf's for overlapping docids
  map<DOCID_T, int>::iterator finder;

  int offset, ll, df, tf, diff;
  long filelen;
  vector<int>::iterator iter;
  InvDocList* list, *prev;
  InvDocInfo* info = new InvDocInfo();
  int overlap;    // the number of times this term got flushed mid docid
  //  LOC_T* locs;
  TERMID_T termid;
  DOCID_T docid;
  while (readers.size() > 1) {
    offset=0;
    working.clear();
    least(&readers, &working);
    list = readers[working.front()]->list;
    termid = list->termID();

   // make sure the ftell is accurate
    indexfile.flush();

    // first, add up the list lengths and df.  
    tfs.clear();
    overlap = 0;
    iter=working.begin();
    prev = readers[*iter]->list;
    ll = prev->length();
    df = prev->docFreq();

    for (iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      ll += list->length();
      df += list->docFreq();
      // count how many times we flushed in the middle of the doc
      list->startIteration();
      list->nextEntry(info);
      if (info->docID() == prev->curDocID()) {
        overlap++;
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) 
          finder->second += info->termCount();
        else 
          tfs[info->docID()] = info->termCount();
      }
      
      prev = list;        
    }

    // for each overlap we reduce the size by 2 more (docid and tf)
    ll = ll - (overlap*2);
    df = df - overlap;
    diff = ((df-1)*2)+4;

    // figure out if we need a new index file or if there's still room
    filelen = (long)indexfile.tellp();
    if (filelen + ((ll+4) *sizeof(int)) > maxfile) {
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

    indexfile.write((const char*) &termid, sizeof(TERMID_T));
    indexfile.write( (const char*) &df, sizeof(int));
    indexfile.write( (const char*) &diff, sizeof(int));
    indexfile.write( (const char*) &ll, sizeof(int));
    // write out the list from the first file pointer
    iter = working.begin();
    prev = readers[*iter]->list;
    prev->startIteration();

    while (prev->hasMore()) {
      prev->nextEntry(info);		
      docid = info->docID();
      tf = info->termCount();

      // peek ahead to see if we're on the last one
      if (!prev->hasMore()) {
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) {
          tf += finder->second;
        }
      }

      indexfile.write((const char*) &docid, sizeof(DOCID_T));
      indexfile.write((const char*) &tf, sizeof(int));

    }  // while prev hasMore

    for (iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      list->startIteration();

      //overlap would occur only for the first one
      list->nextEntry(info);
      // write out docid and termcount only if it hasn't been written already
      if (info->docID() != prev->curDocID()) {
        docid = info->docID();
        tf = info->termCount();
        if (!list->hasMore()) {
          finder = tfs.find(info->docID());
          if (finder != tfs.end()) {
            tf += finder->second;
          }
        }
        indexfile.write((const char*) &docid, sizeof(DOCID_T));
        indexfile.write((const char*) &tf, sizeof(int));    
      }

    while (list->hasMore()) {
      list->nextEntry(info);
      docid = info->docID();
      tf = info->termCount();
      //      locs = info->positions();

      // peek ahead to see if we're on the last one
      if (!list->hasMore()) {
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) {
          tf += finder->second;
        }
      }

      indexfile.write((const char*) &docid, sizeof(DOCID_T));
      indexfile.write((const char*) &tf, sizeof(int));
      //indexfile.write((const char*) locs, sizeof(LOC_T) * info->termCount());

      } 
      prev = list;
    }

    //update this file reader or erase it from available readers
    IndexReader* ir;
    for (iter=working.begin(); iter!=working.end(); iter++) {
      ir = readers[*iter-offset];
      ir->list->resetFree();
      if ( !ir->list->binRead(*(ir->reader)) ) {
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
    // write the current list

    termid = myreader->list->termID();
    ll = myreader->list->length();
    df = myreader->list->docFreq();
    diff = myreader->list->curDocIDdiff();

    indexfile.flush();
    filelen = (long)indexfile.tellp();
    if (filelen + ((ll+4) *sizeof(int)) > maxfile) {
      indexfile.close();
      char* newindex = (char*)malloc(namelen+3);
      sprintf(newindex, "%s%d.%d", name, level, intmed->size());
      intmed->push_back(newindex);
      indexfile.open(newindex, ios::binary | ios::out);
      filelen =0;
    }

    indexfile.write((const char*)&termid, sizeof(TERMID_T));
    indexfile.write((const char*)&df, sizeof(int));     
    indexfile.write((const char*)&diff, sizeof(int));
    indexfile.write((const char*)&ll, sizeof(int));

    myreader->list->startIteration();
    while (myreader->list->hasMore()) {
      myreader->list->nextEntry(info);
      docid = info->docID();
      tf = info->termCount();

      indexfile.write((const char*)&docid, sizeof(DOCID_T));
      indexfile.write((const char*)&tf, sizeof(int));
    }
    myreader->list->resetFree();

    //write the rest 
    while (myreader->list->binRead(*(myreader->reader)) ){
      termid = myreader->list->termID();
      ll = myreader->list->length();
      df = myreader->list->docFreq();
      diff = myreader->list->curDocIDdiff();

      indexfile.flush();
      filelen = (long)indexfile.tellp();
      if (filelen + ((ll+4) *sizeof(int)) > maxfile) {
        indexfile.close();
        char* newindex = (char*)malloc(namelen+3);
        sprintf(newindex, "%s%d.%d", name, level, intmed->size());
        intmed->push_back(newindex);
        indexfile.open(newindex, ios::binary | ios::out);
        filelen=0;
      }

      indexfile.write((const char*)&termid, sizeof(TERMID_T));
      indexfile.write((const char*)&df, sizeof(int));     
      indexfile.write((const char*)&diff, sizeof(int));
      indexfile.write((const char*)&ll, sizeof(int));

      myreader->list->startIteration();
      while (myreader->list->hasMore()) {
	myreader->list->nextEntry(info);
        docid = info->docID();
        tf = info->termCount();

        indexfile.write((const char*)&docid, sizeof(DOCID_T));
        indexfile.write((const char*)&tf, sizeof(int));
      }
      myreader->list->resetFree();
    } //while that file

    myreader->reader->close();
    delete(myreader->list);
    delete(myreader->reader);
    delete(myreader);
	
    readers.clear();
  
  } // if still a file

  indexfile.close();
  delete(info);	
  //remove files already merged
  /*  for (int f=0;f<files->size();f++) {
    remove((*files)[f]);
    }*/

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
    readers[i]->list->binRead(*(readers[i]->reader));
  }

  vector<int> working;  // list of least words
  map<DOCID_T, int> tfs; // combined tf's for overlapping docids
  map<DOCID_T, int>::iterator finder;

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
  int offset, ll, df, tf, diff;
  int ctf = 0;
  long filelen;
  vector<int>::iterator iter;
  InvDocList* list, *prev;
  InvDocInfo* info = new InvDocInfo();
  int overlap;    // the number of times this term got flushed mid docid
  TERMID_T termid;
  DOCID_T docid;
  while (readers.size() > 1) {
    offset=0;
    working.clear();
    least(&readers, &working);
    list = readers[working.front()]->list;
    termid = list->termID();

   // make sure the ftell is accurate
    indexfile.flush();

    // first, add up the list lengths and df.  
    tfs.clear();
    overlap = 0;
    iter=working.begin();
    prev = readers[*iter]->list;
    ll = prev->length();
    df = prev->docFreq();

    for (iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      ll += list->length();
      df += list->docFreq();
      // count how many times we flushed in the middle of the doc
      list->startIteration();
      list->nextEntry(info);
      if (info->docID() == prev->curDocID()) {
        overlap++;
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) 
          finder->second += info->termCount();
        else 
          tfs[info->docID()] = info->termCount();
      }

      prev = list;        
    }

    // for each overlap we reduce the size by 2 more (docid and tf)
    ll = ll - (overlap*2);
    df = df - overlap;
    diff = ((df-1)*2)+4;

    // figure out if we need a new index file or if there's still room
    filelen = (long)indexfile.tellp();
    if (filelen + ((ll+4) *sizeof(int)) > maxfile) {
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

    indexfile.write((const char*) &termid, sizeof(TERMID_T));
    indexfile.write( (const char*) &df, sizeof(int));
    indexfile.write( (const char*) &diff, sizeof(int));
    indexfile.write( (const char*) &ll, sizeof(int));
    // write out the list from the first file pointer
    iter = working.begin();
    prev = readers[*iter]->list;
    prev->startIteration();

    ctf = 0;
    while (prev->hasMore()) {
      prev->nextEntry(info);
      docid = info->docID();
      tf = info->termCount();
      //      locs = info->positions();

      // peek ahead to see if we're on the last one
      if (!prev->hasMore()) {
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) {
          tf += finder->second;
        }
      }
      ctf += tf;
      indexfile.write((const char*) &docid, sizeof(DOCID_T));
      indexfile.write((const char*) &tf, sizeof(int));      
    }  // while prev hasMore


    for (iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      list->startIteration();

      //overlap would occur only for the first one
      list->nextEntry(info);
      // write out docid and termcount only if it hasn't been written already
      if (info->docID() != prev->curDocID()) {
        docid = info->docID();
        tf = info->termCount();
        if (!list->hasMore()) {
          finder = tfs.find(info->docID());
          if (finder != tfs.end()) {
            tf += finder->second;
          }
        }
	ctf += tf;
        indexfile.write((const char*) &docid, sizeof(DOCID_T));
        indexfile.write((const char*) &tf, sizeof(int));    
      }

      while (list->hasMore()) {
	list->nextEntry(info);
	docid = info->docID();
	tf = info->termCount();

	// peek ahead to see if we're on the last one
	if (!list->hasMore()) {
	  finder = tfs.find(info->docID());
	  if (finder != tfs.end()) {
	    tf += finder->second;
	  }
	}
	ctf += tf;
	indexfile.write((const char*) &docid, sizeof(DOCID_T));
	indexfile.write((const char*) &tf, sizeof(int));
      } 
      prev = list;
    }

    fprintf(listing, "%d %d %d %d %d ", termid, fid, filelen, ctf, df);

    //update this file reader or erase it from available readers
    IndexReader* ir;
    for (iter=working.begin(); iter!=working.end(); iter++) {
      ir = readers[*iter-offset];
      ir->list->resetFree();
      if ( !ir->list->binRead(*(ir->reader)) ) {
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
    // write the current list

    termid = myreader->list->termID();
    ll = myreader->list->length();
    df = myreader->list->docFreq();
    diff = myreader->list->curDocIDdiff();

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

    indexfile.write((const char*)&termid, sizeof(TERMID_T));
    indexfile.write((const char*)&df, sizeof(int));     
    indexfile.write((const char*)&diff, sizeof(int));
    indexfile.write((const char*)&ll, sizeof(int));

    ctf = 0;
    myreader->list->startIteration();
    while (myreader->list->hasMore()) {
      myreader->list->nextEntry(info);
      docid = info->docID();
      tf = info->termCount();
      indexfile.write((const char*)&docid, sizeof(DOCID_T));
      indexfile.write((const char*)&tf, sizeof(int));
      ctf += tf;
    }
    myreader->list->resetFree();
    fprintf(listing, "%d %d %d %d %d ", termid, fid, filelen, ctf, df);
    
    //write the rest 
    while (myreader->list->binRead(*(myreader->reader)) ){
      termid = myreader->list->termID();
      ll = myreader->list->length();
      df = myreader->list->docFreq();
      diff = myreader->list->curDocIDdiff();

      //fflush(indexfile);
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

      indexfile.write((const char*)&termid, sizeof(TERMID_T));
      indexfile.write((const char*)&df, sizeof(int));     
      indexfile.write((const char*)&diff, sizeof(int));
      indexfile.write((const char*)&ll, sizeof(int));

      // write the listing
      //fflush(indexfile);
  //    fprintf(indexfile, "%d ", myreader->list->docFreq());

      ctf = 0;
      myreader->list->startIteration();
      while (myreader->list->hasMore()) {
        myreader->list->nextEntry(info);
        docid = info->docID();
        tf = info->termCount();
	ctf += tf;
        indexfile.write((const char*)&docid, sizeof(DOCID_T));
        indexfile.write((const char*)&tf, sizeof(int));
      }
      myreader->list->resetFree();
      fprintf(listing, "%d %d %d %d %d ", termid, fid, filelen, ctf, df);

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
  delete(info);

  //remove files already merged
  /*  for (int f=0;f<files->size();f++) {
    remove((*files)[f]);
    }*/

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

  /*  #ifdef _WIN32
  fs->rdbuf()->pubsetbuf(bp, bytes);
  #else

  #if (_CXXVER == 301 || _CXXVER == 302 )
  fs->rdbuf()->pubsetbuf(bp, bytes);
  #else 
  fs->setbuf(bp, bytes);
  #endif
  #endif
  */
}
