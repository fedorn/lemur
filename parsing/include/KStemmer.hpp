/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _KSTEMMER_HPP
#define _KSTEMMER_HPP
#include "Stemmer.hpp"
#include "indri/KrovetzStemmer.hpp"
#define MAX_FILENAME_LENGTH 125  /* including the full directory path */
namespace lemur 
{
  namespace parse 
  {
    ///  Provides a wrapper to the Krovetz stemmer
    ///  that supports the Stemmer interface, and by
    ///  inheritance, the TextHandler interface.

    class KStemmer : public lemur::api::Stemmer {
    private:
      indri::parse::KrovetzStemmer *stemmer;
    public:
      static const string identifier;

      KStemmer();

      ~KStemmer();  
    
      /// Stem a word using the Krovetz Stemmer.
      char * stemWord(char * word);
    };
  }
}

#endif
