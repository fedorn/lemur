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

#include "PorterStemmer.hpp"

#include <cctype>
#include "common_headers.hpp"

// the Porter stemmer is in C
extern int porter_stem(char * p, int i, int j);


void 
PorterStemmer::stemWord(char * word) {
  // only stem words that begin with a lowercase letter 
  // (don't stem acronyms or names)
  if (islower(*word)) {
    int len = strlen(word);
    len = porter_stem(word, 0, len - 1) + 1;
    word[len] = '\0';
  }
}
