/*==========================================================================
 * Copyright (c) 2001 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#include "ArabicStemmer.hpp"

#include <cctype>
#include "common_headers.hpp"

extern char *arabic_stemdir;
extern void *set_stemmer(char *);
extern void freeWordSets();

ArabicStemmer::ArabicStemmer(const char *stemDir, const char *stemmer) {
  arabic_stemdir = new char[strlen(stemDir) + 1];
  strcpy(arabic_stemdir, stemDir);
  stem_fct = (void (*)(char *, char *)) set_stemmer((char *)stemmer);
}

ArabicStemmer::~ArabicStemmer() {
  freeWordSets();
  delete[](arabic_stemdir);
}  

char * ArabicStemmer::stemWord(char * word) {
  static char buffer[100];
  stem_fct(word, buffer);
  if (buffer[0])
    return buffer;
  else
    return NULL;
}
