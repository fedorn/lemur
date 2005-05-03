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

#ifndef _ARABICSTEMMER_HPP
#define _ARABICSTEMMER_HPP
#include "Stemmer.hpp"
#include "Param.hpp"
/// Arabic Stemmer Parameters
namespace ArabicStemmerParameter {
  /// @name Arabic Stemmer parameters
  //@{ 
  /// stem function to use.
  static lemur::utility::String stemFunc;
  /// path to stemmer data files.
  static lemur::utility::String stemDir;
  //@}
  /// default location of stemmer data files "/usr/local/lemur/arabic_stem_data"
  static lemur::utility::String defaultStemDir("/usr/local/lemur/arabic_stem_data");
  /// default stem function to use "arabic_light10_stop"
  static lemur::utility::String defaultStemFunc("arabic_light10_stop");
  /// get the parameters
  static void get()
  {
    stemFunc = lemur::api::ParamGetString("arabicStemFunc", defaultStemFunc);
    stemDir =  lemur::api::ParamGetString("arabicStemDir", defaultStemDir);
  }
}
namespace lemur
{
  namespace parse
  {
    
    ///  Provides a wrapper to the Arabic stemmer
    ///  that supports the Stemmer interface, and by
    ///  inheritance, the TextHandler interface.
    class ArabicStemmer : public lemur::api::Stemmer {
    public:
      static const string identifier;

      ArabicStemmer(const string &stemDir=ArabicStemmerParameter::defaultStemDir, 
                    const string &stemmer=ArabicStemmerParameter::defaultStemFunc);
      ~ArabicStemmer();
      /// Stem a word using an Arabic stemmer.
      char * stemWord(char * word);
      void writePropertyList(PropertyList* list) const;
    private:
      /// pointer to selected stemmer function.
      void (*stem_fct)(char *, char *);
      string stemfunc;
    };
  }
}

#endif
