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

#include "InvFPTextHandler.hpp"



InvFPTextHandler::InvFPTextHandler(char * filename, int bufferSize,
				   bool countStopWords, int ind) {
  // create index and helper objects  
  if (ind == 0)
    index = new InvPushIndex(filename, bufferSize);
  else index = new InvFPPushIndex(filename, bufferSize);
  //  dp = new DocumentProps();
  dp = NULL;
  term = new InvFPTerm();
  countStopWds = countStopWords;
  docLength = 0;
  pos = 0;
  // set state that is on first doc
  first = true;
}

InvFPTextHandler::~InvFPTextHandler() {
  // end the doc and close the collection
  if (!first) endDoc(); // if we haven't done any yet, don't bother.
  index->endCollection(NULL);
  delete dp;
  delete term;
  delete index;
}

void
InvFPTextHandler::endDoc() {
  dp->length(docLength);
  index->endDoc(dp);
  dp->stringID(NULL);
}

char * 
InvFPTextHandler::handleDoc(char * docno) {
  // finish the old doc  
  if (!first) {
    endDoc();
  } else {
    dp = new DocumentProps();
    first = false;
  }

  // begin the new document
  dp->length(0);  
  docLength = 0;
  pos = 0;
  dp->stringID(docno);
  index->beginDoc(dp);
  return docno;
}

char * 
InvFPTextHandler::handleWord(char * word) {
  if (word != NULL) {
    // ignore words longer than 50 characters
    // this ignores junk in some web pages
    int len = strlen(word);
    if (len <= 50) {
      // index the word
      term->strLength(len);
      term->spelling(word);
      term->position(pos);
      index->addTerm(*term);
      docLength++;
      pos++;
    }
  } else {
    if (countStopWds) docLength++;
    pos++;
  }
  return word;
}


void InvFPTextHandler::setDocManager(const char* mgrID) {
  index->setDocManager(mgrID);
}
