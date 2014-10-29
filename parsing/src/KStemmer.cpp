/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include <cctype>
#include "common_headers.hpp"
#include "Exception.hpp"
#include "KStemmer.hpp"

lemur::parse::KStemmer::KStemmer() {
  stemmer = new indri::parse::KrovetzStemmer();
  iden=identifier;
}

lemur::parse::KStemmer::~KStemmer() {
  delete(stemmer);
}

char * lemur::parse::KStemmer::stemWord(char * word) {
  // only stem words that begin with a lowercase letter 
  // (don't stem acronyms or names)
  if (islower(*word)) {
    return stemmer->kstem_stemmer(word);
  }
  return word;
}
