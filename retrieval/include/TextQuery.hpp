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

#ifndef _TEXTQUERY_HPP
#define _TEXTQUERY_HPP

#include "Document.hpp"
#include "TermQuery.hpp"

namespace lemur 
{
  namespace api 
  {
    
    /// An implementation of TermQuery (wrapper around Document) Best used with query
    /// files and filestream
    /// This text query is an adaptor of Document
    class TextQuery : public TermQuery {
    public:
      TextQuery(const Document &doc) : d(doc) { 
        if (d.getID()) qid=d.getID(); 
        else qid=""; }
      virtual ~TextQuery() {}

      virtual void startTermIteration() const { d.startTermIteration();}
      virtual bool hasMore() const { return d.hasMore();}
      virtual const Term *nextTerm() const { return d.nextTerm();}
    protected:
      const Document &d;
    };
  }
}

#endif
