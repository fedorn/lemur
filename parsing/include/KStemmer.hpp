/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _KSTEMMER_HPP
#define _KSTEMMER_HPP
#include "Stemmer.hpp"
#define MAX_FILENAME_LENGTH 125  /* including the full directory path */


///  Provides a wrapper to the Krovetz stemmer
///  that supports the Stemmer interface, and by
///  inheritance, the TextHandler interface.

class KStemmer : public Stemmer {

public:
  KStemmer(string &datadir);

  ~KStemmer();  
    
  /// Stem a word using the Krovetz Stemmer.
  char * stemWord(char * word);
private:
  char *ddir;
};

#endif
