#include "InvFPIndexMerge.hpp"
#include <string>

InvFPIndexMerge::InvFPIndexMerge(vector<char*>* tf) {
  int i=0;

  for (vector<char*>::iterator p=tf->begin(); p!=tf->end(); p++){
    readers.push_back(new InvFPIndexReader(*p) );
    readers[i++]->readNextList();
  }
}

InvFPIndexMerge::~InvFPIndexMerge() {
  InvFPIndexReader* reader;

  while (readers.size() > 0) {
    reader = readers.front();
    delete(reader);
  }
}

void InvFPIndexMerge::merge(char* name) {
  fprintf(stderr, "Merging files\n");

  vector<int> working;  // list of least words
  map<DOCID_T, int> tfs; // combined tf's for overlapping docids
  map<DOCID_T, int>::iterator finder;
  char* indexname = (char*)malloc(strlen(name)+7);
  char* lookup = (char*)malloc(strlen(name)+11);
  sprintf(indexname, "%s.index", name);
  sprintf(lookup, "%s.invlookup", name);
  FILE* indexfile = fopen(indexname, "wb");
  FILE* listing = fopen(lookup, "wb");

  // we have only one file for now
  int fid = 0;

  int offset, ll, df, k, i;
  vector<int>::iterator iter;
  InvFPDocList* list, *prev;
//  InvFPDocInfo* info = new InvFPDocInfo();
  InvFPDocInfo* info;
  int overlap;    // the number of times this term got flushed mid docid
  LOC_T* locs;
  while (readers.size() > 1) {
    offset=0;
    working.clear();
    least(&working);
    // write out the termid 
    list = readers[working.front()]->currentList();
    fprintf(indexfile, "%d ", list->termID());
    // first, add up the list lengths and df.  write that only once
    tfs.clear();
    overlap = 0;
    iter=working.begin();
    prev = readers[*iter]->currentList();
    ll = prev->length();
    df = prev->docFreq();

    for (iter=iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->currentList();
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
    ll = ll - (overlap*2) + 1;
    df = df - overlap;

    fprintf(indexfile, "%d ", ll);
    // make sure the ftell is accurate
    fflush(indexfile);
    fprintf(listing, "%d %d %d %d\n", list->termID(), fid, ftell(indexfile), ll);
    fprintf(indexfile, "%d ", df);

    // write out the list from the first file pointer
    iter = working.begin();
    prev = readers[*iter]->currentList();
    prev->startIteration();

/*========================HERE=====================================
    for (i=1;i<prev->docFreq();i++) {
//      info = static_cast< InvFPDocInfo* > (prev->nextEntry());
      prev->nextEntry(info);
      fprintf(indexfile, "%d %d ", info->docID(), info->termCount());

      locs = info->positions();
      for (k=0;k<info->termCount();k++) {
        fprintf(indexfile, "%d ", locs[k]);
      }
//      delete(info);
    }
    // we're looking at the last docid
//    info = static_cast< InvFPDocInfo* > (prev->nextEntry());
    prev->nextEntry(info);
    finder = tfs.find(info->docID());
    if (finder != tfs.end())
      fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
    else
       fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
    locs = info->positions();
    for (k=0;k<info->termCount();k++) {
      fprintf(indexfile, "%d ", locs[k]);
    }
  //  delete(info);
/*=================STOPS HERE=====================================*/        

    while (prev->hasMore()) {
      info = static_cast< InvFPDocInfo* > (prev->nextEntry());
//      prev->nextEntry(info);
      // peek ahead to see if we're on the last one
      if (!list->hasMore()) {
        finder = tfs.find(info->docID());
        if (finder != tfs.end())
          fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
        else
          fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
      } else {
        fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
      }

      locs = info->positions();
      for (k=0;k<info->termCount();k++) {
        fprintf(indexfile, "%d ", locs[k]);
      }
      delete(info);
    }

    for (iter=iter++; iter!=working.end(); iter++) {
      list = readers[*iter]->currentList();
      list->startIteration();

      //overlap would occur only for the first one
      info = static_cast< InvFPDocInfo* > (list->nextEntry());
//      list->nextEntry(info);
      // write out docid and termcount only if it hasn't been written already
      if (info->docID() != prev->curDocID()) {
        if (!list->hasMore()) {
//        if (list->docFreq() == 1) {
          finder = tfs.find(info->docID());
          if (finder != tfs.end())
            fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
          else
            fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
        } else {
          fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
        }
      }
      locs = info->positions();
      for (k=0;k<info->termCount();k++) {
        fprintf(indexfile, "%d ", locs[k]);
      }
      delete(info);
/*========================HERE=====================================
      if (list->docFreq() > 1) {
        for (i=2;i<list->docFreq();i++) {
//          info = static_cast< InvFPDocInfo* > (list->nextEntry());
          list->nextEntry(info);
          fprintf(indexfile, "%d %d ", info->docID(), info->termCount());

          locs = info->positions();
          for (k=0;k<info->termCount();k++) {
            fprintf(indexfile, "%d ", locs[k]);
          }
//          delete(info);
        }
        // we're looking at the last docid
//        info = static_cast< InvFPDocInfo* > (list->nextEntry());
        list->nextEntry(info);
        finder = tfs.find(info->docID());
        if (finder != tfs.end())
          fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
        else
          fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
        locs = info->positions();
        for (k=0;k<info->termCount();k++) {
          fprintf(indexfile, "%d ", locs[k]);
        }
//        delete(info);
      }
/*===================STOPS HERE=====================================*/

    while (list->hasMore()) {
      info = static_cast< InvFPDocInfo* > (list->nextEntry());
//        list->nextEntry(info);
        // peek ahead to see if we're on the last one
        if (!list->hasMore()) {
          finder = tfs.find(info->docID());
          if (finder != tfs.end())
            fprintf(indexfile, "%d %d ", info->docID(), info->termCount()+finder->second);
          else
            fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
        } else {
          fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
        }
        locs = info->positions();
        for (k=0;k<info->termCount();k++) {
          fprintf(indexfile, "%d ", locs[k]);
        }
        delete(info);
      } 
      prev = list;
    }

    //update this file reader or erase it from available readers
    for (iter=working.begin(); iter!=working.end(); iter++) {
      if (!readers[*iter-offset]->readNextList()) {
	      readers.erase(readers.begin()+(*iter-offset));
	      //update the other indices
	      offset++;
      }
    }

    /*
    for (j=lworking.begin(); j!=working.end(); j++) {
      indexReader* myreader = readers[*j-offset];
      int* list = myreader->getList();
      for (int k=1;k<myreader->getListLength();k++) {
	      fprintf(indexfile, "%d ", list[k]);
	//				fprintf(stderr, "%d ", list[k]);
      }
      //update this file reader or erase it from available readers
      if (!myreader->readNextWord()) {
	      readers.erase(readers.begin()+(*j-offset));
	      //update the other indices
	      offset++;
      }
    } */
  }
  //grab the last file open if any
  if (readers.size() != 0) {
    InvFPIndexReader* myreader = readers.front();
    // dump it out
    list = myreader->currentList();
    while (list) {
//      list = myreader->currentList();
      fprintf(indexfile, "%d %d ", list->termID(), list->length()+1);
  
      // write the listing
      fflush(indexfile);
      fprintf(listing, "%d %d %d %d\n", list->termID(), fid, ftell(indexfile), list->length()+1);

      fprintf(indexfile, "%d ", list->docFreq());

      list->startIteration();
      while (list->hasMore()) {
        info = static_cast< InvFPDocInfo* > (list->nextEntry());
//        list->nextEntry(info);
        fprintf(indexfile, "%d %d ", info->docID(), info->termCount());
    
        locs = info->positions();
        for (k=0;k<info->termCount();k++) {
          fprintf(indexfile, "%d ", locs[k]);
        }
        delete(info);
      }
      list = myreader->readNextList();
    } //while that file
  } // if still a file
  fclose(indexfile);
  fclose(listing);
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
  list = readers[0]->currentList();
  lid = list->termID();
  ret->push_back(0);

  for (int i=1;i<readers.size(); i++) {
    list = readers[i]->currentList();
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