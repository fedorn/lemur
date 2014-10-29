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

// tnt - 6/2004

#ifndef _STRINGQUERY_HPP
#define _STRINGQUERY_HPP

#include "TermQuery.hpp"
#include "TextHandler.hpp"
#include "common_headers.hpp"

namespace lemur 
{
  namespace parse
  {
    
    /// A query class for handling string queries. Tokenizes at spaces.
    /// Also a TextHandler to begin a chain (call empty constructor, add
    /// other TH, then call parse)
    /// Can be used at the end of a chain also after other TH.
    /// i.e. look at RetMethodManager::runTextQuery

    class StringQuery: public lemur::api::TermQuery, public lemur::api::TextHandler {
    public:
      StringQuery();
      StringQuery(const char* query, const char* qid=NULL);
      StringQuery(const string& query, const string& qid="");
      ~StringQuery();

      /// TermQuery methods
      void startTermIteration() const{iter = 0;}
      bool hasMore() const{ return iter < tokens.size();}
      const lemur::api::Term* nextTerm() const;

      void addTerm(const string& token);
      void addTerm(const char* token);
      void parse(const string& token);

      /// TextHandler methods
      char* handleWord(char* word);
      char* handleSymbol(char* sym);
  
    protected:
      vector<string> tokens;
      mutable int iter;
      mutable lemur::api::Term tt;
    };
 
  }
}

#endif
