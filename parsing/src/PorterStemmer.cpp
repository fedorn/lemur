/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/

/*
 * NAME DATE - COMMENTS
 * pto 2001 - created
 * dmf 9/2002 - modified stemWord to return char*
 *
 *========================================================================*/

#include "PorterStemmer.hpp"

#include <cctype>
#include "common_headers.hpp"


lemur::parse::PorterStemmer::PorterStemmer() : Stemmer() {
  // use the same identifier as we do for the application parameters
  iden = identifier;
  stemmer = new indri::parse::Porter_Stemmer();
}

lemur::parse::PorterStemmer::~PorterStemmer() {
  delete(stemmer);
}

char * lemur::parse::PorterStemmer::stemWord(char * word) {
  // only stem words that begin with a lowercase letter 
  // (don't stem acronyms or names)
  if (islower(*word)) {
    int len = strlen(word);
    len = stemmer->porter_stem(word, 0, len - 1) + 1;
    word[len] = '\0';
  }
  return word;
}
