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

#include <set>


#ifndef _LT_STR_
#define _LT_STR_
/* for the set */
struct lt_str
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};
#endif

#ifndef _WORDSET_H_
#define _WORDSET_H_

///
/// A generic class that provides a neat and easy to use wrapper
/// to a hash_set<char *>.  It can load items in the set from a file
/// (one item per line).  It also handles its own memory conveniently.
///
///
class WordSet {

public:
  WordSet();
  /// Create object, and load set from file.
  WordSet(char * filename);
  ~WordSet();

  /// Load a set from file.  Does not clear the set first.
  void load(char * filename);
  /// Add a word (char *) to the set.
  void add(char * word);
  /// Check for the existence of a word in the set.
  bool contains(char * word);

  /// Empty the set and free memory.
  void clear();


protected:
  
  /// The set used to store words.
  set<char *, lt_str> words;


};

#endif
