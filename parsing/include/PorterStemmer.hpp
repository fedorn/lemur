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
#include "indri/Porter_Stemmer.hpp"
#ifndef _PORTERSTEMMER_HPP
#define _PORTERSTEMMER_HPP
namespace lemur 
{
  namespace parse
  {
    ///
    ///  Provides a wrapper to the Porter stemmer
    ///  that supports the Stemmer interface, and by
    ///  inheritance, the TextHandler interface.

    class PorterStemmer : public lemur::api::Stemmer {
    private:
      indri::parse::Porter_Stemmer *stemmer;
    public:
      static const string identifier;

      PorterStemmer();
      ~PorterStemmer();

      /// Stem a word using the Porter Stemmer.
      char * stemWord(char * word);

    };
  }
}

#endif
