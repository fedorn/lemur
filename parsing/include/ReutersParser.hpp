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


#ifndef _REUTERSPARSER_HPP
#define _REUTERSPARSER_HPP
#include "Parser.hpp"
#include "TextHandler.hpp"
namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  Parses documents in TREC format.  Does case folding
    ///  for words that are not in the acronym list.  Contraction suffixes 
    ///  and possessive suffixes are stripped.  
    ///
    ///  U.S.A., USA's, and USAs are converted to USA.  Does not recognize
    ///  acronyms with numbers.
    ///
    ///  The following fields are parsed: text, headline, title
    ///  
    ///

    // Source code in reutersparser.lex

    class ReutersParser : public lemur::api::Parser {

    public:
      static const string identifier;

      ReutersParser();

      /// Parse a file.
      void parseFile (const string &filename);
      void parseBuffer (char * buf, int len);
      long fileTell()const;
 
    private:
      /// Actual parsing action flow
      void doParse();

      /// The state of the parser.
      int state;

    };
  }
}

#endif
