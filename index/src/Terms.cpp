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


#include "Terms.hpp"

Terms* Terms::setOfSets = NULL;
int    Terms::nSetsOpen = 0;
  
static void my_assert(int exp, char * msg) {
  if (exp == 0) {
    cerr << msg << endl;
    exit(0);
  }
}

Terms *Terms::construct(const string &path_) {
  // access only through here 
  int n = NSetsOpen();
  for (int i=0; i<n; i++) 
    //    if (!strcmp(SetOfSets()[i].path, path_)) return &(SetOfSets()[i]);
    if (SetOfSets()[i].path == path_) return &(SetOfSets()[i]);
  SetOfSets()[n].open(path_);
  NSetsOpen()++;
  return &(SetOfSets()[n]);
}  
  
int Terms::tokenize(const char *buff, Array<unsigned short> &tokenized) const { 
  Array<int> tokenizedInt;
  tokenize(buff, tokenizedInt);
  tokenized.Grow(tokenizedInt.Size());
  for (int i=0; i<(int) tokenizedInt.Size(); i++) {
    tokenized[i] = (unsigned short) tokenizedInt[i];
    my_assert((tokenizedInt[i]<65536), "overflow error?");
  }
  return tokenized.Size();
}

int Terms::tokenize(const char *buff, Array<int> &tokenized) const { 
  // caller is responsible for deallocating 'tokenized'
  char *shadow = strdup(buff);
  tokenized.Grow(strlen(buff));
  char *p = strtok(shadow, "\n\t ");
  int nTerms=0;
  for (;p; nTerms++) {
    int term = terms[p];
    if (term==-1) term = getIndexOfOOV();
    tokenized[nTerms] = term;
    p = strtok(NULL, "\n\t ");
  }
  tokenized.Grow(nTerms);
  free (shadow); 
  return nTerms;
}

void Terms::open(const string &path_) {
  path = path_; 
  char buff[65536];
  ifstream ifs(path.c_str(), ios::in);
  if (!ifs) {
    cerr << "Can't open "<< path <<endl;
    abort(); 
  }
  terms.open(100);
  while (ifs.peek()!=EOF) {
    ifs >> buff;
    if (strspn(buff,"\t\n ")==strlen(buff)) continue; // blank
    terms += (string) buff;
  }     
  cerr << "Terms: Read "<< terms.size() 
       << " spellings from " << path <<endl;
}
