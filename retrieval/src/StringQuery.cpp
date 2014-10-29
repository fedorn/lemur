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

lemur::parse::StringQuery::StringQuery() {
  iter=0;
}

lemur::parse::StringQuery::StringQuery(const char* query, const char* id) {
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

lemur::parse::StringQuery::StringQuery(const string& query, const string& id) {
  istringstream ss(query);
  string tok;
  while (ss >> tok) {
    tokens.push_back(tok);
  }
  if (!id.empty())
    qid = id;
}

lemur::parse::StringQuery::~StringQuery() {
}

void lemur::parse::StringQuery::parse(const string& query) {
  istringstream ss(query);
  string tok;
  while (ss >> tok) {
    tokens.push_back(tok);
    // if anyone's listening, pass this along
    if (textHandler)
      textHandler->foundWord((char*)tokens.back().c_str());
  }
}

const lemur::api::Term* lemur::parse::StringQuery::nextTerm() const {
  tt.spelling(tokens[iter]);
  iter++;
  return &tt;
}

void lemur::parse::StringQuery::addTerm(const string& token) {
  tokens.push_back(token);
}

void lemur::parse::StringQuery::addTerm(const char* token) {
  if (token)
    tokens.push_back(token);
}

char* lemur::parse::StringQuery::handleWord(char* word) {
  addTerm(word);
  return word;
}

char* lemur::parse::StringQuery::handleSymbol(char* sym) {
  addTerm(sym);
  return sym;
}
