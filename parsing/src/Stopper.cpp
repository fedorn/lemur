/*==========================================================================
 * Copyright (c) 2000-2005 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#include "Stopper.hpp"

#include <iostream>
#include <fstream>

#define MAXLINE 500

lemur::api::Stopper::Stopper() {
  cat = category;
  iden = identifier;
}

lemur::api::Stopper::Stopper(const string &filename) {
  cat = category;
  iden = identifier;
  try {
    load(filename);
  } catch (Exception &ex) {
    LEMUR_RETHROW(ex, "Stopwords file could not be loaded.");
  }
}


bool 
lemur::api::Stopper::stopWord(const char * word) const {
  // return true if the word is in the stopword list
  return contains(word);
}

char * 
lemur::api::Stopper::handleWord(char * word) {
  if (word != NULL && stopWord(word)) {
    word = NULL;
  }
  return word;
}

void
lemur::api::Stopper::writePropertyList(lemur::parse::PropertyList* list) const{
  TextHandler::writePropertyList(list);
  lemur::parse::Property p("stopwords");
  p.setValue(lastfile);
  list->setProperty(&p);
}
