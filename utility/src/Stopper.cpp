
#include "Stopper.hpp"

#include <iostream>
#include <fstream>

#define MAXLINE 500

Stopper::Stopper() {
}

Stopper::Stopper(const string &filename) {
  load(filename);
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
