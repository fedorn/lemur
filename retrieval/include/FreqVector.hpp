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


#ifndef _FREQVECTOR_HPP
#define _FREQVECTOR_HPP

#include "ByteHash.hpp"
#include "Index.hpp"
#include "CSet.hpp"

class FreqCount {
public:
  int key;
  unsigned int Hash() const { return ByteHash((char *)(&key),sizeof(int),0); }
  unsigned int hash() const { return Hash(); }
  bool operator==(const FreqCount count) { return count.key == this->key;} 
};
  
/// Abstract class that represents a frequency vector accessible 
/// through an integer key

class FreqVector {
public:
  virtual bool find(int ind, int &freq)=0;
  virtual void startIteration()=0;
  virtual bool hasMore()=0;
  virtual void nextFreq(int &id, int &freq)=0;
  virtual int size()=0;
};

class HashFreqVector : public FreqVector, public CSet<FreqCount, int> {
public:
  HashFreqVector() : CSet<FreqCount, int>(300) {}
  HashFreqVector(Index &index, int docID) : CSet<FreqCount, int>(300) {
    TermInfoList *tList = index.termInfoList(docID);
    TermInfo *info;
    while (tList->hasMore()) {
      info = tList->nextEntry();
      static FreqCount ct;
      ct.key = info->id();
      add(ct, info->count());
    }
    delete tList;

  }
  virtual ~HashFreqVector() {};
  virtual bool find(int ind, int &freq) {
    static FreqCount c;
    c.key = ind;
    freq = count(c);
    if (freq==0) return false;
  }
  virtual void startIteration() {
    i=0;
  }
  virtual bool hasMore() { 
    return (i < size());
  }
  virtual void nextFreq(int &id, int &freq) {
    // get the i-th element
    static FreqCount c;
    id = (ISet<FreqCount>::operator[](i)).key;
    c.key = id;
    freq = count(c)    ;
    i++;
  }

  virtual int size() {
    return (ISet<FreqCount>::size());
  }
protected:
  int i;
};

#endif /* _FREQVECTOR_HPP */






