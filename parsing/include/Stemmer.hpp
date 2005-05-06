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
namespace lemur 
{
  namespace api 
  {

    ///  \brief A generic interface for Stemmers.
    /*!  They must implement the TextHandler interface.
     */
    class Stemmer : public TextHandler {

    public:
      static const string category;
      static const string identifier;

      Stemmer() { cat=category; iden=identifier;}

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
  }
}

#endif
