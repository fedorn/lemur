/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
 */

#ifndef _TEXTHANDLERMGR_HPP
#define _TEXTHANDLERMGR_HPP

#include "Parser.hpp"
#include "Stopper.hpp"
#include "Stemmer.hpp"
namespace lemur 
{
  namespace api 
  {
    
    /// \brief Factory class for creating a Parser, Stemmer, or Stopper. 
    /*!
        Object types will be created from the specified 
        name. If no name is specified, will try to get name from parameter 
        stack.
        People adding new TH objects can do so here, and existing applications
        using this THManager will be able to make use of them without 
        modification.
    **/
    class TextHandlerManager {
    public:
      /// current known types are trec, web, chinese, chinesechar and arabic
      /// if nothing is passed in and nothing is on param stack, return NULL
      /// checks for "docFormat" parameter for type 
      /// checks for "acronyms" parameter for acronyms list
      static Parser* createParser(string type="", string acros="");

      /// current known types are proter, krovetz, and arabic
      /// if nothing is passed in and nothing is on param stack, return NULL
      /// checks for "stemmer" parameter for type 
      /// "arabicStemDir" for datadir
      /// "arabicStemFunc" for func
      static Stemmer* createStemmer(string type="", string datadir="", string func="");

      /// as of now, there is only 1 stopper so no type is passed in. 
      /// this can easily be changed.
      /// "stopwords" param for filename
      static Stopper* createStopper(string filename="");
    };
  }
}

#endif //_TEXTHANDLERMGR_HPP
