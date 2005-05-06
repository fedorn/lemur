/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _FLOATFREQVECTOR_HPP
#define _FLOATFREQVECTOR_HPP

#include "FreqVector.hpp"
namespace lemur 
{
  namespace utility
  {
    /// Record with frequency information to be stored in a hash table
    class FloatFreqCounter {
    public:
      lemur::api::TERMID_T key;
      // used for termid->f mappings, no need to compute a hash
      unsigned int Hash() const { return (unsigned int)key; }
      unsigned int hash() const { return Hash(); }
      bool operator==(const FloatFreqCounter count) { return count.key == this->key;} 
    };
    /// Vector of FloatFreqCounter elements.
    class FloatFreqVector : public FreqVector, 
                            public CSet<FloatFreqCounter, double> {
    public:
      FloatFreqVector() : CSet<FloatFreqCounter, double>(1000) {s2 = 0;}
      /// Construct a document frequency vector based on the counts stored in an index
      FloatFreqVector(const lemur::api::Index &index, 
                      lemur::api::DOCID_T docID);
      FloatFreqVector(const lemur::api::Index &index, 
                      lemur::api::TermInfoList *tList);
      FloatFreqVector(const lemur::api::Index &index, 
                      vector<lemur::api::DOCID_T> &dids);
      FloatFreqVector(FloatFreqVector *old);

      virtual ~FloatFreqVector() {};
      virtual bool find(lemur::api::TERMID_T ind, double &freq) const;
      virtual void startIteration() const {  i=0; }
      virtual bool hasMore() const {  return (i < size()); }
      virtual void nextFreq(lemur::api::TERMID_T &id, double &freq) const;
      /// return the total number of non-zero elements in the vector
      virtual int size() const {    return (ISet<FloatFreqCounter>::size()); }

      virtual void nextFreq (lemur::api::TERMID_T &id, int &freq)  const;
      virtual bool find(lemur::api::TERMID_T ind, int &freq) const ;

      void addVal(lemur::api::TERMID_T id, int cnt);
      void addVal(lemur::api::TERMID_T id, double val);
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
  }
}

#endif /* _FLOATFREQVECTOR_HPP */
