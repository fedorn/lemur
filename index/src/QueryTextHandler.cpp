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

#include "QueryTextHandler.hpp"
#include <cctype>
#include <cstring>
#include <cstdlib>

lemur::parse::QueryTextHandler::QueryTextHandler() {
  index = NULL;
}

char * 
lemur::parse::QueryTextHandler::handleWord(char * word) {
  if (word != NULL) {
    int id = index->term(word);

    // make a copy of the word in uppercase
    char * uword = strdup(word);
    int l = strlen(word);
    for (int i = 0; i < l; i++) uword[i] = toupper(uword[i]);
    int idu = index->term(uword);
    
    // add the uppercase word to the query if it occurs
    // in the index more often than the original word
    if (index->termCount(idu) > index->termCount(id)) {      
      textHandler->foundWord(uword);
    } 
    free (uword);
  }
  return word;
}

void 
lemur::parse::QueryTextHandler::setIndex(const lemur::api::Index * ind) {
  index = ind;
}
