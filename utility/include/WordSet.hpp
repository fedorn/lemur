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


#ifndef _LT_STR
#define _LT_STR

#include "common_headers.hpp"

/* for the set */
struct lt_str
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};
#endif

#ifndef _WORDSET_HPP
#define _WORDSET_HPP

/// A generic class that provides a neat and easy to use wrapper to a hash_set<char *>.
/*! 
 A generic class that provides a neat and easy to use wrapper
 to a hash_set<char *>.  It can load items in the set from a file
 (one item per line).  It also handles its own memory conveniently.

*/

class WordSet {

public:
  WordSet();
  /// Create object, and load set from file.
  WordSet(const string &filename);
  ~WordSet();

  /// Load a set from file.  Does not clear the set first.
  void load(const string &filename);
  /// Add a word (char *) to the set.
  void add(const char * word);
  /// Check for the existence of a word in the set.
  bool contains(const char * word) const;

  /// Empty the set and free memory.
  void clear();


protected:
  
  /// The set used to store words.
  /// once a word is stored, we do not change it
  set<char *, lt_str> words;


};

#endif
