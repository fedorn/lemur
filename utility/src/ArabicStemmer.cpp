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

extern void *set_stemmer(char *);
extern void freeWordSets();

ArabicStemmer::ArabicStemmer(const char *stemDir, const char *stemmer) {
  stemmer_dir = new char[255];
  stemmer_dir[0]='\0';
  strcat(stemmer_dir, "ARABIC_STEM_DIR=");
  strcat(stemmer_dir, stemDir);
  if(putenv(stemmer_dir))
    cerr << "putenv can not set ARABIC_STEM_DIR" << endl;
  stem_fct = (void (*)(char *, char *)) set_stemmer((char *)stemmer);
}

ArabicStemmer::~ArabicStemmer() {
  freeWordSets();
  delete[](stemmer_dir);
}  

char * ArabicStemmer::stemWord(char * word) {
  static char buffer[100];
  stem_fct(word, buffer);
  if (buffer[0])
    return buffer;
  else
    return NULL;
}
