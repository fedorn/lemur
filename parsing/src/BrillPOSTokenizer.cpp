/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
#include "BrillPOSTokenizer.hpp"

BrillPOSTokenizer::BrillPOSTokenizer() {
  splitter='/';
  pos.setName("POS");
}

BrillPOSTokenizer::BrillPOSTokenizer(char s) {
  splitter = s;
  pos.setName("POS");
}

char* BrillPOSTokenizer::handleWord(char* word, char* original, PropertyList* list){
  char* term = strtok(word, &splitter);
  char* tag = strtok(NULL, &splitter);
  if (tag) {
    pos.setValue(tag);
    list->setProperty(&pos);
  } 
  return term;
}

