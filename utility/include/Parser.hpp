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

#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "TextHandler.hpp"
#include "WordSet.hpp"

///
///  Provides a generic parser interface.  Assumes
///  that the parser uses an acronym list.  If, when developing
///  your parser, you do not use an acronym list, you can
///  just provide an empty implementation of the setAcroList
///  function.
///

class Parser : public TextHandler {

public:

  Parser() {
    acros = NULL;
  }

  /// Parse a file.  
  /// use parseFile.  this method will be deprecated in future
  virtual void parse(char * filename) { parseFile(filename); } ;
  
  /// Parse a file.
  virtual void parseFile(char* filename) = 0;

  /// Parse a buffer.  
  virtual void parseBuffer(char * buf, int len) = 0;

  /// Set the acronym list.  Can be an empty implementation if
  /// the parser is not designed to deal with acronyms by
  /// using a list.
  virtual void setAcroList(WordSet * acronyms)  {
    acros = acronyms;
  }

  /// return the current byte position of the file being parsed
  virtual long fileTell() = 0;

  /// return the byte position at the beginning of the current document
  virtual long getDocBytePos() { return docpos; }

protected: 
  /// Checks to see if the word is in the acronym list.
  /// Returns false if the list is not set.
  bool isAcronym(char * word) {
    if (acros != NULL) 
      return acros->contains(word);
    return false;
  }

  long docpos; 

private:
  /// The acronym list.
  WordSet * acros;

};

#endif
