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
#include "TextHandler.hpp"

#ifndef _PARSER_HPP
#define _PARSER_HPP

#include "WordSet.hpp"

///
///  Provides a generic parser interface.  Supports the 
///  TextHandler interface as a source (so foundDoc and
///  foundWord have empty implementations).  Also assumes
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
  virtual void parse(char * filename) = 0;

  /// Set the acronym list.  Can be an empty implementation if
  /// the parser is not designed to deal with acronyms by
  /// using a list.
  virtual void setAcroList(WordSet * acronyms)  {
    acros = acronyms;
  }


protected: 
  /// Checks to see if the word is in the acronym list.
  /// Returns false if the list is not set.
  bool isAcronym(char * word) {
    if (acros != NULL) 
      return acros->contains(word);
    return false;
  }

private:
  /// The acronym list.
  WordSet * acros;

};

#endif
