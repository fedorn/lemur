/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _STRINGQUERY_HPP
#define _STRINGQUERY_HPP


#include "Document.hpp"
#include "RetrievalMethod.hpp"

/// A term in a StringQuery
class StringTokenTerm : public TokenTerm {
public:
  StringTokenTerm(char * tokterm) { term = strdup(tokterm); }
  ~StringTokenTerm() { free(term); }
  virtual const char * spelling() { return term; }
private:
  char * term;
};

/// A string query takes a char * and splits it by spaces
class StringQuery : public Query {
public:
  StringQuery(char * qid, char * qry) { 
    if (qid != NULL) { 
      tid = strdup(qid); 
    } else {
      tid = NULL;
    }
    if (qry != NULL) {
      query = strdup(qry); 
    } else {
      query = NULL;
    }
  }
  virtual ~StringQuery() { 
    free(tid);
    free(query); 
  }
  virtual char * id() { return tid; }
  virtual void startTermIteration() { 
    current = strtok(query, " ");
  }
  virtual bool hasMore() {
    if (current == NULL) {
      return false;
    }
    return true;
  }
  virtual TokenTerm * nextTerm() { 
    if (!hasMore()) {
      return NULL;
    }
    TokenTerm * term = new StringTokenTerm(current);
    current = strtok(NULL, " ");
    return term;
  }
protected:
  char * tid;
  char * query;
  char * current;
};



#endif
