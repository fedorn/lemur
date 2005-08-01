/*==========================================================================
 * Copyright (c) 2001 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


#include "ArabicStemmer.hpp"

lemur::parse::ArabicStemmer::ArabicStemmer(const string &stemFunc) {
  stemmer = new Arabic_Stemmer(stemFunc);
}

lemur::parse::ArabicStemmer::~ArabicStemmer() {
  delete(stemmer);
}  

char * lemur::parse::ArabicStemmer::stemWord(char * word) {
  // presumes that word < 100 chars.
  stemmer->stemTerm(word, stem);
  if (stem[0])
    return stem;
  else
    return NULL;
}
