#include "InvFPIndexMerge.hpp"
#include <string>

InvFPIndexMerge::InvFPIndexMerge(vector<char*>* tf) {
  int i=0;
  for (vector<char*>::iterator p=tf->begin(); p!=tf->end(); p++){
    readers.push_back(new IndexReader);
//    readers.push_back(new InvFPIndexReader(*p) );
//    readers[i++]->readNextList();
    readers[i]->reader = new ifstream();
    readers[i]->reader->open(*p, ios::in | ios::binary);
    readers[i]->list = new InvFPDocList();
    readers[i]->list->binRead(*(readers[i]->reader));
    i++;
  }
}

InvFPIndexMerge::~InvFPIndexMerge() {
/*  InvFPIndexReader* reader;

  while (readers.size() > 0) {
    reader = readers.front();
    delete(reader);
  } */
}

void InvFPIndexMerge::merge(char* name) {
  fprintf(stderr, "Merging files\n");

  vector<int> working;  // list of least words
  map<DOCID_T, int> tfs; // combined tf's for overlapping docids
  map<DOCID_T, int>::iterator finder;
  char* indexname = (char*)malloc(strlen(name)+strlen(INVINDEX)+1);
  char* lookup = (char*)malloc(strlen(name)+strlen(INVLOOKUP)+1);
  sprintf(indexname, "%s%s", name, INVINDEX);
  sprintf(lookup, "%s%s", name, INVLOOKUP);
//  FILE* indexfile = fopen(indexname, "wb");
  ofstream indexfile;
  indexfile.open(indexname, ios::binary | ios::out);
  FILE* listing = fopen(lookup, "wb");

  // we have only one file for now
  int fid = 0;

  int offset, ll, df, tf;
  vector<int>::iterator iter;
  InvFPDocList* list, *prev;
//  InvFPDocInfo* info = new InvFPDocInfo();
  InvFPDocInfo* info;
  int overlap;    // the number of times this term got flushed mid docid
  LOC_T* locs;
  TERMID_T termid;
  DOCID_T docid;
  while (readers.size() > 1) {
    offset=0;
    working.clear();
    least(&working);
    // write out the termid 
    list = readers[working.front()]->list;
//    fprintf(indexfile, "%d ", list->termID());
    termid = list->termID();

   // make sure the ftell is accurate
//    fflush(indexfile);
    indexfile.flush();

    // write out the lookup first
    fprintf(listing, "%d %d %d ", termid, fid, (long)indexfile.tellp());    
    indexfile.write((const char*) &termid, sizeof(TERMID_T));

    // first, add up the list lengths and df.  write that only once
    tfs.clear();
    overlap = 0;
    iter=working.begin();
    prev = readers[*iter]->list;
    ll = prev->length();
    df = prev->docFreq();

    for (iter=iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      ll += list->length();
      df += list->docFreq();
      // count how many times we flushed in the middle of the doc
      list->startIteration();
      info = static_cast< InvFPDocInfo* > (list->nextEntry());
//      list->nextEntry(info);
      if (info->docID() == prev->curDocID()) {
        overlap++;
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) 
          finder->second += info->termCount();
        else 
          tfs[info->docID()] = info->termCount();
      }
      delete(info);
      prev = list;        
    }

    // for each overlap we reduce the size by 2 more (docid and tf)
    ll = ll - (overlap*2);
    df = df - overlap;

//    fprintf(indexfile, "%d ", ll);
//    fprintf(listing, "%d %d %d %d\n", list->termID(), fid, ftell(indexfile), ll);
//    fprintf(listing, "%d %d %d %d\n", list->termID(), fid, indexfile.tellp(), ll);
//    fprintf(indexfile, "%d ", df);
    indexfile.write( (const char*) &df, sizeof(int));
    // PLACEHOLDER
    int diff = 0;
    indexfile.write( (const char*) &diff, sizeof(int));
    indexfile.write( (const char*) &ll, sizeof(int));
    // write out the list from the first file pointer
    iter = working.begin();
    prev = readers[*iter]->list;
    prev->startIteration();

    while (prev->hasMore()) {
      info = static_cast< InvFPDocInfo* > (prev->nextEntry());
//      prev->nextEntry(info);
     
      docid = info->docID();
      tf = info->termCount();
      locs = info->positions();

      // peek ahead to see if we're on the last one
      if (!prev->hasMore()) {
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) {
//          fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
          tf += finder->second;
        }
      }

      indexfile.write((const char*) &docid, sizeof(DOCID_T));
      indexfile.write((const char*) &tf, sizeof(int));
      indexfile.write((const char*) locs, sizeof(LOC_T) * info->termCount());

      /*
      for (k=0;k<info->termCount();k++) {
        fprintf(indexfile, "%d ", locs[k]);
      }
      */
      
      delete(info);
    }  // while prev hasMore

    for (iter=iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->list;
      list->startIteration();

      //overlap would occur only for the first one
      info = static_cast< InvFPDocInfo* > (list->nextEntry());
//      list->nextEntry(info);
      // write out docid and termcount only if it hasn't been written already
      if (info->docID() != prev->curDocID()) {
        docid = info->docID();
        tf = info->termCount();
        if (!list->hasMore()) {
          finder = tfs.find(info->docID());
          if (finder != tfs.end()) {
//          fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
            tf += finder->second;
          }
        }
        indexfile.write((const char*) &docid, sizeof(DOCID_T));
        indexfile.write((const char*) &tf, sizeof(int));    
      }
      locs = info->positions();
      indexfile.write((const char*) locs, sizeof(LOC_T) * info->termCount());

        /*
        for (k=0;k<info->termCount();k++) {
        fprintf(indexfile, "%d ", locs[k]);
      }
      */

      delete(info);

    while (list->hasMore()) {
      info = static_cast< InvFPDocInfo* > (list->nextEntry());
//        list->nextEntry(info);
      docid = info->docID();
      tf = info->termCount();
      locs = info->positions();

      // peek ahead to see if we're on the last one
      if (!list->hasMore()) {
        finder = tfs.find(info->docID());
        if (finder != tfs.end()) {
//          fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
          tf += finder->second;
        }
      }

      indexfile.write((const char*) &docid, sizeof(DOCID_T));
      indexfile.write((const char*) &tf, sizeof(int));
      indexfile.write((const char*) locs, sizeof(LOC_T) * info->termCount());

      /*
      for (k=0;k<info->termCount();k++) {
        fprintf(indexfile, "%d ", locs[k]);
      }
      */

      delete(info);
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
//    fprintf(indexfile, "%d %d ", myreader->list->termID(), myreader->list->length()+1);

    termid = myreader->list->termID();
    ll = myreader->list->length();
    df = myreader->list->docFreq();
    int diff = 0;

    //fflush(indexfile);
    indexfile.flush();
//    fprintf(listing, "%d %d %d %d\n", myreader->list->termID(), fid, ftell(indexfile), myreader->list->length()+1);
    fprintf(listing, "%d %d %d ", myreader->list->termID(), fid, (long)indexfile.tellp());
    
    indexfile.write((const char*)&termid, sizeof(TERMID_T));
//    fprintf(indexfile, "%d ", myreader->list->docFreq());   
    indexfile.write((const char*)&df, sizeof(int));     
    indexfile.write((const char*)&diff, sizeof(int));
    indexfile.write((const char*)&ll, sizeof(int));

    myreader->list->startIteration();
    while (myreader->list->hasMore()) {
      info = static_cast< InvFPDocInfo* > (myreader->list->nextEntry());
//        list->nextEntry(info);
//      fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
      docid = info->docID();
      tf = info->termCount();
      locs = info->positions();
    /*
      for (k=0;k<info->termCount();k++) {
        fprintf(indexfile, "%d ", locs[k]);
      }
      */
      indexfile.write((const char*)&docid, sizeof(DOCID_T));
      indexfile.write((const char*)&tf, sizeof(int));
      indexfile.write((const char*)locs, sizeof(LOC_T) * tf);
      delete(info);
    }
    myreader->list->resetFree();

    //write the rest 
    while (myreader->list->binRead(*(myreader->reader)) ){
//    fprintf(indexfile, "%d %d ", myreader->list->termID(), myreader->list->length()+1);
      termid = myreader->list->termID();
      ll = myreader->list->length();
      df = myreader->list->docFreq();
      int diff = 0;

      //fflush(indexfile);
      indexfile.flush();
  //    fprintf(listing, "%d %d %d %d\n", myreader->list->termID(), fid, ftell(indexfile), myreader->list->length()+1);
      fprintf(listing, "%d %d %d ", myreader->list->termID(), fid, (long)indexfile.tellp());

      indexfile.write((const char*)&termid, sizeof(TERMID_T));
//    fprintf(indexfile, "%d ", myreader->list->docFreq());     
      indexfile.write((const char*)&df, sizeof(int));     
      indexfile.write((const char*)&diff, sizeof(int));
      indexfile.write((const char*)&ll, sizeof(int));

      // write the listing
      //fflush(indexfile);
  //    fprintf(indexfile, "%d ", myreader->list->docFreq());

      myreader->list->startIteration();
      while (myreader->list->hasMore()) {
        info = static_cast< InvFPDocInfo* > (myreader->list->nextEntry());
  //        list->nextEntry(info);
  //      fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
        docid = info->docID();
        tf = info->termCount();
        locs = info->positions();
      /*
        for (k=0;k<info->termCount();k++) {
          fprintf(indexfile, "%d ", locs[k]);
        }
        */
        indexfile.write((const char*)&docid, sizeof(DOCID_T));
        indexfile.write((const char*)&tf, sizeof(int));
        indexfile.write((const char*)locs, sizeof(LOC_T) * tf);
        delete(info);
      }
      myreader->list->resetFree();
    } //while that file

    myreader->reader->close();
    delete(myreader->list);
    delete(myreader->reader);
    delete(myreader);
    readers.clear();
  
  } // if still a file
//  fclose(indexfile);
  indexfile.close();
  fclose(listing);
  free(indexname);
  free(lookup);
//  delete(info);
}

void InvFPIndexMerge::least(vector<int>* ret) {
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

  InvFPDocList* list;

  int lid, id;
  list = readers[0]->list;
  lid = list->termID();
  ret->push_back(0);

  for (int i=1;i<readers.size(); i++) {
    list = readers[i]->list;
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