/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
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
      return stemWord(word);
    }
    return word;
  }
  /// Stem a word.  May overwrite the char *
  /// passed in.
  virtual char *stemWord(char * word) = 0;

};

#endif
