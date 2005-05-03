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

#ifndef _TERMQUERY_HPP
#define _TERMQUERY_HPP

#include "Term.hpp"
#include "RetrievalMethod.hpp" // for Query
namespace lemur 
{
  namespace api
  {
    /// Abstract interface for a query containing a sequence of Terms
    class TermQuery : public Query {
    public:
      virtual ~TermQuery() {}

      virtual void startTermIteration() const = 0;
      virtual bool hasMore() const = 0;
      virtual const Term *nextTerm() const = 0;
    };
  }
}

#endif
