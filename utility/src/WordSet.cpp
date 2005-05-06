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

#include "WordSet.hpp"

#include <iostream>
#include <fstream>

#define MAXLINE 500

#ifndef NULL
#define NULL 0
#endif

lemur::utility::WordSet::WordSet() {
}

lemur::utility::WordSet::WordSet(const string &filename) {
  load(filename);
  lastfile = filename;
}

lemur::utility::WordSet::~WordSet() {
  // free all of the words in the set
  clear();
}


void
lemur::utility::WordSet::load(const string &filename) {
  lastfile = filename;
  ifstream ifstr(filename.c_str());
  // return if the file can't be opened
  if (ifstr == NULL) {
    LEMUR_THROW(LEMUR_IO_ERROR, "Cannot open file");
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
lemur::utility::WordSet::add(const char * word) {
  // insert the word into the set if it isn't already there
  if (!contains(word)) {
    words.insert(strdup(word));
  }
}

bool 
lemur::utility::WordSet::contains(const char * word) const{
  // check to see if the word is in the set
  // can't make this a set of const char* because we need to free it
  set<char *, lt_str>::const_iterator it = words.find((char*)word);
  return (it != words.end());
}

void 
lemur::utility::WordSet::clear() {
  // free memory and empty the set
  set<char *, lt_str>::iterator curr = words.begin();
  while (curr != words.end()) {
    free (*curr);
    curr++;
  }
  words.clear();
}
