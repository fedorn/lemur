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

#ifndef _ARABICSTEMMER_HPP
#define _ARABICSTEMMER_HPP
#include "Stemmer.hpp"
#include "Param.hpp"
/// Arabic Stemmer Parameters
namespace ArabicStemmerParameter {
  /// @name Arabic Stemmer parameters
  //@{ 
  /// stem function to use.
  static String stemFunc;
  /// path to stemmer data files.
  static String stemDir;
  //@}
  /// default location of stemmer data files "/usr/local/lemur/arabic_stem_data"
  static String defaultStemDir("/usr/local/lemur/arabic_stem_data");
  /// default stem function to use "arabic_light10_stop"
  static String defaultStemFunc("arabic_light10_stop");
  /// get the parameters
  static void get()
  {
    stemFunc = ParamGetString("arabicStemFunc", defaultStemFunc);
    stemDir =  ParamGetString("arabicStemDir", defaultStemDir);
  }
}
  
///  Provides a wrapper to the Arabic stemmer
///  that supports the Stemmer interface, and by
///  inheritance, the TextHandler interface.
class ArabicStemmer : public Stemmer {
public:
  static const string identifier;

  ArabicStemmer(const string &stemDir=ArabicStemmerParameter::defaultStemDir, 
		const string &stemmer=ArabicStemmerParameter::defaultStemFunc);
  ~ArabicStemmer();
  /// Stem a word using an Arabic stemmer.
  char * stemWord(char * word);

private:
  /// pointer to selected stemmer function.
  void (*stem_fct)(char *, char *);
};

#endif
