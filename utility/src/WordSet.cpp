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

#include "WordSet.hpp"

#include <iostream>
#include <fstream>

#define MAXLINE 500

#ifndef NULL
#define NULL 0
#endif

WordSet::WordSet() {
}

WordSet::WordSet(const string &filename) {
  load(filename);
}

WordSet::~WordSet() {
  // free all of the words in the set
  clear();
}


void
WordSet::load(const string &filename) {
  ifstream ifstr(filename.c_str());
  // return if the file can't be opened
  if (ifstr == NULL) {
    return;
  }
 
  char word[MAXLINE];
  word[MAXLINE-1] = '\0';

  // load in the words
  while (! ifstr.eof()) {
    ifstr >> word;
    add(word);
  }

  ifstr.close();
}

void
WordSet::add(const char * word) {
  // insert the word into the set if it isn't already there
  if (!contains(word)) {
    words.insert(strdup(word));
  }
}

bool 
WordSet::contains(const char * word) const{
  // check to see if the word is in the set
  // can't make this a set of const char* because we need to free it
  set<char *, lt_str>::const_iterator it = words.find((char*)word);
  return (it != words.end());
}

void 
WordSet::clear() {
  // free memory and empty the set
  set<char *, lt_str>::iterator curr = words.begin();
  while (curr != words.end()) {
    free (*curr);
    curr++;
  }
  words.clear();
}
