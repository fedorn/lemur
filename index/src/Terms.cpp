 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Terms.hpp"

Terms* Terms::setOfSets = NULL;
int    Terms::nSetsOpen = 0;
  
Terms *Terms::construct(const char *path_) {
  // access only through here 
  int n = NSetsOpen();
  for (int i=0; i<n; i++) 
    if (!strcmp(SetOfSets()[i].path, path_)) return &(SetOfSets()[i]);
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
    assert(tokenizedInt[i]<65536); // overflow error? 
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

void Terms::open(const char *path_) {
  path = path_; 
  char buff[65536];
  ifstream ifs(path, ios::in);
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
