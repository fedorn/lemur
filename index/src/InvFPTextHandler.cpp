/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "InvFPTextHandler.hpp"

lemur::parse::InvFPTextHandler::InvFPTextHandler(const string &filename, int bufferSize,
                                                 bool countStopWords, int ind) {
  // create index and helper objects  
  if (ind == 0)
    index = new lemur::index::InvPushIndex(filename, bufferSize);
  else index = new lemur::index::InvFPPushIndex(filename, bufferSize);
  //  dp = new DocumentProps();
  dp = NULL;
  term = new lemur::index::InvFPTerm();
  countStopWds = countStopWords;
  docLength = 0;
  pos = 0;
  // set state that is on first doc
  first = true;
}

lemur::parse::InvFPTextHandler::~InvFPTextHandler() {
  // end the doc and close the collection
  if (!first) endDoc(); // if we haven't done any yet, don't bother.

  endCollection();

  delete dp;
  delete term;
  delete index;
}

void
lemur::parse::InvFPTextHandler::endDoc() {
  dp->length(docLength);
  index->endDoc(dp);
  dp->stringID(NULL);
}

void
lemur::parse::InvFPTextHandler::endCollection() {
  // write out properties from chain, excluding this 
  BasicCollectionProps* props = new BasicCollectionProps();
  if (!props) {
    cout << "could not initialize" << endl;
    return;
  }
  
  lemur::api::TextHandler* temphandler = this->getPrevHandler();
  while (temphandler) {
    temphandler->writePropertyList(props);
    temphandler = temphandler->getPrevHandler();
  }

  index->endCollection(props);
  delete (props);
}

char * 
lemur::parse::InvFPTextHandler::handleDoc(char * docno) {
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
lemur::parse::InvFPTextHandler::handleWord(char * word) {
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


void lemur::parse::InvFPTextHandler::setDocManager(const string &mgrID) {
  index->setDocManager(mgrID);
}
