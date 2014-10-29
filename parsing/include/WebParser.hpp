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

#ifndef _WEBPARSER_HPP
#define _WEBPARSER_HPP

#include "Parser.hpp"
#include "TextHandler.hpp"
#include "LinkedPropertyList.hpp"

namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  Parses documents in NIST's Web TREC format.  Does case folding
    ///  for words that are not in the acronym list.  Contraction suffixes 
    ///  and possessive suffixes are stripped.  
    ///
    ///  U.S.A., USA's, and USAs are converted to USA.  Does not recognize
    ///  acronyms with numbers.
    ///
    ///  The DOCHDR is ignored.
    ///
    ///  Text in &lt;script&gt; tags is ignored.  Text in HTML &lt;!-- --&gt; comments 
    ///  is ignored.

    // Source code in webparser.lex

    class WebParser : public lemur::api::Parser {

    public:
      static const string identifier;

      WebParser();

      /// Parse a file.
      void parseFile(const string &filename);
  
      void parseBuffer(char * buf, int len);
 
      long fileTell() const;

    private:
      /// Actual parsing action flow
      void doParse();

      /// The state of the parser.
      int state;

      /// keep a property for being and end of elements
      Property begelem;
      Property endelem;

      /// list
      LinkedPropertyList proplist;
    };
  }
}

#endif
