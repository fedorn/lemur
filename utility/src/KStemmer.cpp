/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "KStemmer.hpp"

#include <cctype>
#include "common_headers.hpp"

// the Krovetz stemmer is in C
extern char *kstem_stemmer(char *);
extern char *stemdir;

KStemmer::KStemmer(string &datadir) {
  stemdir = new char[datadir.length() + 1];
  strcpy(stemdir, datadir.c_str());
}

KStemmer::~KStemmer() {
  delete[](stemdir);
}

char * KStemmer::stemWord(char * word) {
  // only stem words that begin with a lowercase letter 
  // (don't stem acronyms or names)
  if (islower(*word)) {
    return kstem_stemmer(word);
  }
  return word;
}
