/*==========================================================================
 * Copyright (c) 2003 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
#include "TextHandler.hpp"

#ifndef _BRILLPOSTOK_HPP
#define _BRILLPOSTOK_HPP

///
///  This TextHandler parses tokens that have been put through
///  Brill's POS tagger.  This is usually of the format "word/POS".  
///  This TH will split the token at the delimiter, send the word as 
///  is along the pipeline with the POS added as a Property.  TextHandlers
///  further down the chain can access the POS by getting a Property
///  named "POS" from the PropertyList. Generally, this Parser should
///  be chained after a TextHandler tokenizing parser, such as the 
///  WebParser, and before sending to Stopper or Stemmer.
///  

class BrillPOSTokenizer : public TextHandler {

public:
  /// make a new POSTokenizer with default split character "/"
  BrillPOSTokenizer();

  /// make a new POSTokenizer with a different splitting character
  BrillPOSTokenizer(char s);

  /// set a new delimiter character to split into tokens
  void setDelimiter(char s) {splitter = s;};

  ///  split the token, send the word as is
  ///  along the pipeline with the POS added as a Property
  char *  handleWord(char* word, char* original, PropertyList* list);

protected:
  char splitter;
  // keep setting one property.  PropertyList copies.
  Property pos;
};

#endif
