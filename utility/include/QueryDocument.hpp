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

// tnt - 2/2003

#include "Document.hpp" 
#include "TextHandler.hpp"
#include "common_headers.hpp"

#ifndef QUERYDOC_HPP
#define QUERYDOC_HPP


/// A convenience class for dealing with "interactive" queries. Creates
/// a Document object compatible with TextQuery and StructQuery. Also a
/// TH so it can be chained at the end of stoppers, stemmers, InqueryOpParser,
/// (etc.) for query processing.

class QueryToken: public TokenTerm {
public:
  QueryToken() {}
  virtual ~QueryToken(){}
  virtual const char *spelling() { return str; }
  friend class QueryDocument;
  friend class StrStructQuery;
private:
  char *str;
};

class QueryDocument: public Document, public TextHandler {
public:
  QueryDocument();
  ~QueryDocument();
  void startTermIteration() {iter = 0;}
  bool hasMore() { return iter < tokens.size();}
  void skipToEnd() {iter = tokens.size();}

  /// get the next term. do not delete TT returned
  TokenTerm* nextTerm();
  void addTerm(const char* token);
  char* handleWord(char *word);
  char* handleSymbol(char *sym);

  char *getID() const {return id;}
  void setID(const char* idstr) {id = strdup(idstr);}

private:
  char* id;
  vector<char*> tokens;
  int iter;
  QueryToken tt;
};

#endif
