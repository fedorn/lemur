/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#include "QueryDocument.hpp"

QueryDocument::QueryDocument() {
  iter = 0;
  id = NULL;
}

QueryDocument::~QueryDocument() {
  while (!tokens.empty()){
    free(tokens.back()); 
    tokens.pop_back();
  }
  free(id);
}

const TokenTerm* QueryDocument::nextTerm() const{
  tt.str = tokens[iter];
  iter++;
  return &tt;
}

void QueryDocument::addTerm(const char* token) {
  if (!token) 
    return; 
  tokens.push_back(strdup(token));
}

char* QueryDocument::handleWord(char *word) {
  if (word) 
    addTerm(word); 
  return word;
}

char* QueryDocument::handleSymbol(char *sym) {
  addTerm(sym); 
  return sym;
}
