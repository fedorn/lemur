/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

#include "Stemmer.hpp"

#ifndef _PORTERSTEMMER_HPP
#define _PORTERSTEMMER_HPP


///
///  Provides a wrapper to the Porter stemmer
///  that supports the Stemmer interface, and by
///  inheritance, the TextHandler interface.

class PorterStemmer : public Stemmer {

public:
  static const string identifier;

  PorterStemmer();

  /// Stem a word using the Porter Stemmer.
  char * stemWord(char * word);

};

#endif
