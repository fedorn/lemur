
#include "Stopper.hpp"

#include <iostream>
#include <fstream>

#define MAXLINE 500

Stopper::Stopper() {
  cat = category;
  iden = identifier;
}

Stopper::Stopper(const string &filename) {
  cat = category;
  iden = identifier;
  try {
    load(filename);
  } catch (Exception &ex) {
    LEMUR_RETHROW(ex, "Stopwords file could not be loaded.");
  }
}


bool 
Stopper::stopWord(const char * word) const {
  // return true if the word is in the stopword list
  return contains(word);
}

char * 
Stopper::handleWord(char * word) {
  if (word != NULL && stopWord(word)) {
    word = NULL;
  }
  return word;
}

void
Stopper::writePropertyList(PropertyList* list) const{
  TextHandler::writePropertyList(list);
  Property p("stopwords");
  p.setValue(lastfile);
  list->setProperty(&p);
}
