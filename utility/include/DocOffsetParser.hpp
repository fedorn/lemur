/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _DOCOFFSETPARSER_HPP
#define _DOCOFFSETPARSER_HPP
#include "Parser.hpp"
#include "Match.hpp"

///  The DocOffsetParser collects the byte offsets for the
/// terms in a document
class DocOffsetParser : public TextHandler {

public:
  /// initialize with the given parser
  DocOffsetParser(Parser *parser) : p(parser) { 
    p->setTextHandler(this);
  }
  
  virtual ~DocOffsetParser(){}

  /// Add start and byte offsets for this term to the list of offsets
  virtual char *handleWord(char * word) {
    if (word != NULL) {
      int end = p->fileTell() - 1;
      int start = (end - strlen(word)) + 1;
      Match m;
      m.start = start;
      m.end = end;    
      offsets.push_back(m);
    }
    return word;
  }
  ///Parse a string, collecting the token offsets.
  virtual void parseString(char *buffer) {
    offsets.clear();
    p->parseBuffer(buffer, strlen(buffer));
  }
  /// Return the termIds vector  
  vector <Match> getOffsets() { return offsets; }
private:
  /// The offsets vector  
  vector <Match> offsets;
  Parser *p;
};

#endif
