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

#ifndef _IDENTIPARSER_HPP
#define _IDENTIPARSER_HPP
#include "Parser.hpp"
#include "TextHandler.hpp"
#include "LinkedPropertyList.hpp"

namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  Parses documents in with similar document separation tags NIST's Web format.  
    ///  &lt;DOC&gt;&lt;/DOC&gt; around documents and &lt;DOCNO&gt;&lt;/DOCNO&gt; around docids.
    ///  This parser recognizes named entity tags from the Identifinder tagger and
    ///  passed them along as properties. For each tag X, also adds in b_X and e_X 
    ///  to the first and last token of each entity.
    ///  For example, "Carnegie Mellon University" was identified as a place,
    ///  it would be parsed with the following properties:
    ///  Carnegie [b_place] [place] Mellon [place] University [e_place] [place]
    ///  A single token entity, like Madonna would be
    ///  Madonna [b_person] [person] [e_person]
    ///  Does case folding for words that are not in the acronym list.  
    ///  Contraction suffixes and possessive suffixes are stripped.  
    ///
    ///  U.S.A., USA's, and USAs are converted to USA.  Does not recognize
    ///  acronyms with numbers.
    ///
    // Source code in IdentifinderParser.l


#define BEGIN_PREFIX "B_"
#define END_PREFIX "E_"
    // for simplicity, make both prefixes the same length
#define PREFIX_LEN 2

    class IdentifinderParser : public lemur::api::Parser {

    public:
      static const string identifier;

      IdentifinderParser();

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
      Property tag;   // entity tag
      Property btag;  // the begin tag
      Property etag;  // the end tag

  
      /// list
      LinkedPropertyList proplist;
    };
  }
}

#endif
