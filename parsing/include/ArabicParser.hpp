/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _ARABICPARSER_HPP
#define _ARABICPARSER_HPP

#include "Parser.hpp"
#include "TextHandler.hpp"

namespace lemur 
{
  namespace parse
  {
    
    ///  Parses arabic documents in NIST's TREC format, windows CP1256 encoding.
    ///
    ///  The following fields are parsed: TEXT, HL, HEAD, HEADLINE,
    ///  LP, TTL, HEADER, FOOTER.
    class ArabicParser : public lemur::api::Parser {

    public:
      static const string identifier;

      ArabicParser();

      /// Parse a file.
      void parseFile (const string &filename);

      /// Parse a buffer of len length
      void parseBuffer (char * buf, int len);

      /// Gives current byte position offset into file being parsed.
      /// Don't use with parseBuffer
      long fileTell() const;
 
    private:
      /// Actual parsing action flow
      void doParse();
      /// The state of the parser.
      int state;
    };
  }
}

#endif
