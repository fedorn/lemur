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
				   bool countStopWords) {
  // create index and helper objects  
  index = new InvFPPushIndex(filename, bufferSize);
  dp = new DocumentProps();
  term = new InvFPTerm();

  countStopWds = countStopWords;

  docLength = 0;
  pos = 0;
  // set state that is on first doc
  bool first = true;
}

InvFPTextHandler::~InvFPTextHandler() {
  // end the doc and close the collection
  endDoc();
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
    // ignore words longer than 20 characters
    int len = strlen(word);
    if (len <= 20) {
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


