/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
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
#include <string.h>

KeyfileTextHandler::KeyfileTextHandler( KeyfileIncIndex* index ) :
  _index( index )
{
  dp = NULL;
  docLength = 0;
  pos = 0;
  first = true;
  countStopWds = false;
}

KeyfileTextHandler::~KeyfileTextHandler() {
  // end the doc and close the collection
  if (!first) {
    endDoc(); // if we haven't done any yet, don't bother.
    _index->endCollection(NULL);
  }
  if( dp )
    delete dp;
}

void
KeyfileTextHandler::endDoc() {
  dp->length(docLength);
  _index->endDoc(dp);
  dp->stringID(NULL);
}

char * 
KeyfileTextHandler::handleDoc(char * docno) {
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
KeyfileTextHandler::handleWord(char * word) {
  InvFPTerm term;

  if (word != NULL) {
    // don't ignore words based on length
    int len = strlen(word);
    // index the word
    term.strLength(len);
    term.spelling(word);
    term.position(pos);
    _index->addTerm(term);
    docLength++;
    pos++; // this should happen even if >20.
  } else {
    if (countStopWds) docLength++;
    pos++;
  }
  return word;
}

void KeyfileTextHandler::setDocManager(const char* mgrID) {
    _index->setDocManager(mgrID);
  }
