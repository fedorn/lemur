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

/// A string query takes a char * and splits it by spaces
class StringQuery : public Query {
public:
  StringQuery(const char * qid, const char * qry) { 
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
  virtual const char * id() const { return tid; }
  virtual void startTermIteration() const { 
    current = strtok(query, " ");
  }
  virtual bool hasMore() const {
    if (current == NULL) {
      return false;
    }
    return true;
  }
  virtual Term * nextTerm() const { 
    if (!hasMore()) {
      return NULL;
    }
    Term * term = new Term();
    term->copyspelling(current);
    current = strtok(NULL, " ");
    return term;
  }
protected:
  char * tid;
  char * query;
  mutable char * current;
};



#endif
