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

#include "Parser.hpp"
#include "TextHandler.hpp"
#include "LinkedPropertyList.hpp"

#ifndef _TRECPARSER_HPP
#define _TRECPARSER_HPP
namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  Parses documents in NIST's TREC format.  Does case folding
    ///  for words that are not in the acronym list.  Contraction suffixes 
    ///  and possessive suffixes are stripped.  
    ///
    ///  U.S.A., USA's, and USAs are converted to USA.  Does not recognize
    ///  acronyms with numbers.
    ///
    ///  The following fields are parsed: TEXT, HL, HEAD, HEADLINE,
    ///  LP, TTL
    ///  
    ///

    // Source code in trecparser.lex

    class TrecParser : public lemur::api::Parser {
  
    public:
      static const string identifier;

      TrecParser();

      /// Parse a file.
      void parseFile(const string &filename);

      /// Parse a buffer of len length
      void parseBuffer(char * buf, int len);

      /// Gives current byte position offset into file being parsed.
      /// Don't use with parseBuffer
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
