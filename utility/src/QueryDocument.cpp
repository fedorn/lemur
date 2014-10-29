/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#include "QueryDocument.hpp"

lemur::api::QueryDocument::QueryDocument() {
  iter = 0;
  id = NULL;
}

lemur::api::QueryDocument::~QueryDocument() {
  while (!tokens.empty()){
    free(tokens.back()); 
    tokens.pop_back();
  }
  free(id);
}

const lemur::api::Term* lemur::api::QueryDocument::nextTerm() const{
  tt.spelling(tokens[iter]);
  iter++;
  return &tt;
}

void lemur::api::QueryDocument::addTerm(const char* token) {
  if (!token) 
    return; 
  tokens.push_back(strdup(token));
}

char* lemur::api::QueryDocument::handleWord(char *word) {
  if (word) 
    addTerm(word); 
  return word;
}

char* lemur::api::QueryDocument::handleSymbol(char *sym) {
  addTerm(sym); 
  return sym;
}
