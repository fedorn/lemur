
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
