/*==========================================================================
 * Copyright (c) 2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#include "StringQuery.hpp"
#include <sstream>

StringQuery::StringQuery() {
  iter=0;
}

StringQuery::StringQuery(const char* query, const char* id) {
  char* copy = strdup(query);
  char* tok = strtok(copy, " ");
  while (tok) {
    tokens.push_back(tok);
    tok = strtok(NULL, " ");
  }
  free(copy);

  if (id)
    qid = id;
}

StringQuery::StringQuery(const string& query, const string& id) {
  istringstream ss(query);
  string tok;
  while (ss >> tok) {
    tokens.push_back(tok);
  }
  if (!id.empty())
    qid = id;
}

StringQuery::~StringQuery() {
}

void StringQuery::parse(const string& query) {
  istringstream ss(query);
  string tok;
  while (ss >> tok) {
    tokens.push_back(tok);
    // if anyone's listening, pass this along
    if (textHandler)
      textHandler->foundWord((char*)tokens.back().c_str());
  }
}

const Term* StringQuery::nextTerm() const {
  tt.spelling(tokens[iter]);
  iter++;
  return &tt;
}

void StringQuery::addTerm(const string& token) {
  tokens.push_back(token);
}

void StringQuery::addTerm(const char* token) {
  if (token)
    tokens.push_back(token);
}

char* StringQuery::handleWord(char* word) {
  addTerm(word);
  return word;
}

char* StringQuery::handleSymbol(char* sym) {
  addTerm(sym);
  return sym;
}
