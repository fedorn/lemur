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


#include "KStemmer.hpp"

#include <cctype>
#include "common_headers.hpp"

// the Krovetz stemmer is in C
extern char *kstem_stemmer(char *);

char * KStemmer::stemWord(char * word) {
  return kstem_stemmer(word);
}
