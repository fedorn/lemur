/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
#include "TextHandler.hpp"
#include "WordSet.hpp"

#ifndef _STOPPER_HPP
#define _STOPPER_HPP

///
///  Provides a stopword list that can be chained with a Parser using
///  the TextHandler class. 
///

class Stopper : public WordSet, public TextHandler {

public:
  Stopper();
  /// Create the stopword list, loading it from file.
  /// One word per line.
  Stopper(char * filename);

  /// Checks to see if a word is a stopword.
  bool stopWord(char * word);


  /// Will replace stopwords with a NULL pointer.
  /// Words not in the stopword list
  /// are passed on as is.
  char *  handleWord(char * word);


};

#endif
