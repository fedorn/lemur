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

#include "Parser.hpp"

#include "TextHandler.hpp"

#ifndef _TRECPARSER_HPP
#define _TRECPARSER_HPP

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

class TrecParser : public Parser {

public:
  TrecParser();

  /// Parse a file.
  void parseFile(char * filename);

  /// Parse a buffer of len length
  void parseBuffer(char * buf, int len);

  /// Gives current byte position offset into file being parsed.
  /// Don't use with parseBuffer
  long fileTell();

private:
  /// Actual parsing action flow
  void doParse();

  /// The state of the parser.
  int state;

};

#endif
