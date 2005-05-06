/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
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

#ifndef _BRILLPOSPARSER_HPP
#define _BRILLPOSPARSER_HPP
namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  Parses documents in with similar document separation tags NIST's Web format.  
    ///  &lt;DOC&gt;&lt;/DOC&gt; around documents and &lt;DOCNO&gt;&lt;/DOCNO&gt; around docids.
    ///  recognizes tokens with "/" slashes in them, which is the default separator
    ///  for Brill's part of speech tagger. Use with BrillPOSTokenizer.
    ///  This parser also recognizes ./.  ?/. and !/. as end of sentence markers and
    ///  sends along a [eos] token to be indexed.
    ///  Does case folding for words that are not in the acronym list.  
    /// Contraction suffixes and possessive suffixes are stripped.  
    ///
    ///  U.S.A., USA's, and USAs are converted to USA.  Does not recognize
    ///  acronyms with numbers.
    ///

    // Source code in BrillPOSParser.l

    class BrillPOSParser : public lemur::api::Parser {

    public:
      static const string identifier;

      BrillPOSParser();

      /// Parse a file.
      void parseFile(const string &filename);
  
      void parseBuffer(char * buf, int len);
 
      long fileTell() const;

    private:
      /// Actual parsing action flow
      void doParse();

      /// The state of the parser.
      int state;

      /// count position of word in document
      int poscount;

      /// keep one property and change values
      Property wordpos;
      Property tag;
  
      /// list
      LinkedPropertyList proplist;
    };
  }
}

#endif
