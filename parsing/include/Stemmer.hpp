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

#ifndef  _STEMMER_HPP
#define _STEMMER_HPP

/// Porter stemmer

/*! 
  A C++ wrapper for a Porter stemmer originally implemented in C. 
  The original stemmer was written by B. Frakes and C. Cox in 1986, and 
later changed   by C. Fox in 1990 and 1991.

*/

class Stemmer {
public:
  static int stemWord(char *word);
};


#endif  /* _STEMMER_HPP  */
/* EOF */
