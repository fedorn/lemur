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



/// Hash table of term spellings


// ======================================================================
// Terms.hpp
// Adam Berger
// Stores a hash table of recognized term spellings
// ======================================================================

#ifndef _TERMSH_
#define _TERMSH_

#include <iostream.h>
#include <fstream.h>

#include "String.hpp"
#include "ISet.hpp"
#include "Array.hpp"

static const char *OOV_SPELLING  = "[OOV]";
static const char *NULL_SPELLING = "[NULL]";
static const int MAX_SETS_OPEN = 128;


class Terms {
public:
  static Terms *construct(const char *path_);
  const char *operator[](int idx) const { return terms[idx]; }
  int operator[](const char *t)   const 
    { int k=terms[t]; if (k==-1) return getIndexOfOOV(); else return k;} 
  int size()                      const { return terms.size(); }
  int getIndexOfOOV()             const { return terms[OOV_SPELLING];  }
  int getIndexOfNullTerm()        const { return terms[NULL_SPELLING]; }
  const char *getPath()           const { return path; }  
  static const char * getOOVSpelling() { return OOV_SPELLING; }
  int tokenize(const char *buff, Array<int> &tokenized) const;

  // for back-compatibility with 089 code 
  int tokenize(const char *buff, Array<unsigned short> &tokenized) const;

public:
  void open(const char *path_);

private:
  ISet<String> terms;
  String path; 
  
private:
  static Terms* setOfSets;
  static int nSetsOpen;
  static Terms *SetOfSets() { 
    if (setOfSets==NULL) setOfSets = new Terms [MAX_SETS_OPEN]; 
    return setOfSets; 
  }
  static int &NSetsOpen() { return nSetsOpen; } 
};

class Source : public Terms {
};

class Target : public Terms {
};


#endif
