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

#include <cctype>
#include "common_headers.hpp"
#include "Exception.hpp"
#include "KStemmer.hpp"
// the Krovetz stemmer is in C
extern char *kstem_stemmer(char *);
extern char *stemdir;
extern int read_dict_info();

KStemmer::KStemmer(string &datadir) {
  stemdir = new char[datadir.length() + 1];
  strcpy(stemdir, datadir.c_str());
  if (read_dict_info()) {
    cerr << "Failure reading kstemmer data files. Check KstemmerDir = " <<
      datadir << " parameter." << endl;
    LEMUR_THROW( LEMUR_IO_ERROR, "Error opening stemmer datafiles");
  }
  iden=identifier;
}

KStemmer::~KStemmer() {
  delete[](stemdir);
}

char * KStemmer::stemWord(char * word) {
  // only stem words that begin with a lowercase letter 
  // (don't stem acronyms or names)
  if (islower(*word)) {
    return kstem_stemmer(word);
  }
  return word;
}
