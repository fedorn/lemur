/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// KeyfileTextHandler
//
// 19 September 2003 - tds
//
// Initial version copied from InvFPTextHandler
//

#include "KeyfileTextHandler.hpp"
#include "InvFPTerm.hpp"
#include "DocumentProps.hpp"
#include "KeyfileIncIndex.hpp"
#include <string>

lemur::parse::KeyfileTextHandler::KeyfileTextHandler( lemur::index::KeyfileIncIndex* index, 
                                                      bool countStops) :
  _index( index ), countStopWds(countStops)
{
  dp = NULL;
  docLength = 0;
  pos = 0;
  first = true;
}

lemur::parse::KeyfileTextHandler::~KeyfileTextHandler() {
  // end the doc and close the collection
  if (!first) {
    endDoc(); // if we haven't done any yet, don't bother.
    endCollection();
  }
  if( dp )
    delete dp;
}

void
lemur::parse::KeyfileTextHandler::endDoc() {
  dp->length(docLength);
  _index->endDoc(dp);
  dp->stringID(NULL);
}

void
lemur::parse::KeyfileTextHandler::endCollection() {
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

  _index->endCollection(props);
  delete (props);
}

char * 
lemur::parse::KeyfileTextHandler::handleDoc(char * docno) {
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

  pos = 0; /*start at 0*/

  dp->stringID(docno);
  _index->beginDoc(dp);
  return docno;
}

char * 
lemur::parse::KeyfileTextHandler::handleWord(char * word) {
  lemur::index::InvFPTerm term;
  bool success = false;
  if (word != NULL) {
    // don't ignore words based on length
    int len = strlen(word);
    // index the word
    term.strLength(len);
    term.spelling(word);
    term.position(pos);
    // addTerm returns true if term is added.
    success =  _index->addTerm(term);
  }
  // term was added or we are counting stop words.
  if (success || countStopWds) docLength++;
  pos++;
  return word;
}

void lemur::parse::KeyfileTextHandler::setDocManager(const string &mgrID) {
  _index->setDocManager(mgrID);
}
