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

#include "Stemmer.hpp"

#ifndef _PORTERSTEMMER_HPP
#define _PORTERSTEMMER_HPP


///
///  Provides a wrapper to the Porter stemmer
///  that supports the Stemmer interface, and by
///  inheritance, the TextHandler interface.

class PorterStemmer : public Stemmer {

public:
  
  /// Stem a word using the Porter Stemmer.
  void stemWord(char * word);

};

#endif
