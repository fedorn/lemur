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

#ifndef _STEMMER_HPP
#define _STEMMER_HPP


///
///  A generic interface for Stemmers.  They should
///  support the TextHandler interface.
///

class Stemmer : public TextHandler {

public:
  
  virtual char * handleWord(char * word) {
    if (word != NULL) {
      stemWord(word);
    }
    return word;
  }

  /// Stem a word.  Overwrites the char *
  /// passed in.
  virtual void stemWord(char * word) = 0;

};

#endif
