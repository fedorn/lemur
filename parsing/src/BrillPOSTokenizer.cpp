/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/
#include "BrillPOSTokenizer.hpp"

lemur::parse::BrillPOSTokenizer::BrillPOSTokenizer() {
  splitter='/';
  pos.setName("POS");
}

lemur::parse::BrillPOSTokenizer::BrillPOSTokenizer(char s) {
  splitter = s;
  pos.setName("POS");
}

char* lemur::parse::BrillPOSTokenizer::handleWord(char* word, const char* original, PropertyList* list){
  char* term = strtok(word, &splitter);
  char* tag = strtok(NULL, &splitter);
  if (tag) {
    pos.setValue(tag);
    list->setProperty(&pos);
  } 
  return term;
}

