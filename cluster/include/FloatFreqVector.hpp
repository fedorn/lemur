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


#ifndef _FLOATFREQVECTOR_HPP
#define _FLOATFREQVECTOR_HPP

#include "FreqVector.hpp"

/// Record with frequency information to be stored in a hash table
class FreqCounter {
public:
  int key;
  // used for termid->f mappings, no need to compute a hash
  unsigned int Hash() const { return (unsigned int)key; }
  unsigned int hash() const { return Hash(); }
  bool operator==(const FreqCounter count) { return count.key == this->key;} 
};

class FloatFreqVector : public FreqVector, public CSet<FreqCounter, double> {
public:
  FloatFreqVector() : CSet<FreqCounter, double>(1000) {s2 = 0;}
  /// Construct a document frequency vector based on the counts stored in an index
  FloatFreqVector(const Index &index, int docID);
  FloatFreqVector(const Index &index, TermInfoList *tList);
  FloatFreqVector(const Index &index, vector<int> &dids);
  FloatFreqVector(FloatFreqVector *old);

  virtual ~FloatFreqVector() {};
  virtual bool find(int ind, double &freq) const;
  virtual void startIteration() const {  i=0; }
  virtual bool hasMore() const {  return (i < size()); }
  virtual void nextFreq(int &id, double &freq) const;
  /// return the total number of non-zero elements in the vector
  virtual int size() const {    return (ISet<FreqCounter>::size()); }

  virtual void nextFreq (int &id, int &freq)  const;
  virtual bool find(int ind, int &freq) const ;

  void addVal(int id, int cnt);
  void addVal(int id, double val);
  double sum() const; 
  double sum2() const;
  void weigh(const double *vals);
  void weigh(double vals); 
  void addVec(const FloatFreqVector *v2);
  void subtract(const FloatFreqVector *v2); 
  double dotProd(const FloatFreqVector *v2);

protected:
  mutable int i;
  mutable double s2;
  
};

#endif /* _FLOATFREQVECTOR_HPP */
