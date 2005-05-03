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

#include "LemurMemParser.hpp"


// Create the Lemur memory "parser"
lemur::distrib::LemurMemParser::LemurMemParser(const lemur::api::Index * ind) {
  index = ind;
}

// "Parse" the document
void 
lemur::distrib::LemurMemParser::parse(doc_t * doc) {
  // The doc is really an array if integer term ids.
  int * intdoc = (int *) doc->doc;
  int intlen = doc->len * sizeof(char) / sizeof(int);

  // Pass on document boundaries.
  if (textHandler != NULL) textHandler->foundDoc((char *)(doc->id.c_str()));

  int * loc = intdoc;
  int * end = intdoc + intlen;
  while (loc < end) {
    // Look up the word for the term id and pass it on.
    if (textHandler != NULL) 
      textHandler->foundWord((char *)(index->term(*loc).c_str()));
    loc++;
  }
  
  
}
