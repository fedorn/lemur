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


#ifndef _FREQVECTOR_HPP
#define _FREQVECTOR_HPP

#include "ByteHash.hpp"
#include "Index.hpp"
#include "CSet.hpp"
namespace lemur
{
  namespace utility
  {
    
    /// Record with frequency information to be stored in a hash table
    class FreqCount {
    public:
      lemur::api::TERMID_T key;
      unsigned int Hash() const { return ByteHash((char *)(&key),sizeof(lemur::api::TERMID_T),0); }
      unsigned int hash() const { return Hash(); }
      bool operator==(const FreqCount count) { return count.key == this->key;} 
    };
  
    /// \brief Abstract class that represents a frequency vector accessible 
    /// through an integer key

    class FreqVector {
    public:

      /// Find the frequency of a word identified by the index "ind"
      virtual bool find(lemur::api::TERMID_T ind, int &freq) const=0;
      virtual void startIteration()const=0;
      virtual bool hasMore()const=0;
      /// Fetch the next frequency entry
      virtual void nextFreq(lemur::api::TERMID_T &id, int &freq)const=0;
      virtual int size()const=0;
    };


    /// Representation of a frequency vector with a hash table
    class HashFreqVector : public FreqVector, public CSet<FreqCount, int> {
    public:
      HashFreqVector() : CSet<FreqCount, int>(300) {}
      /// Construct a document frequency vector based on the counts stored in an index
      HashFreqVector(const lemur::api::Index &index, lemur::api::DOCID_T docID);

      virtual ~HashFreqVector() {};

  
      virtual bool find(lemur::api::DOCID_T ind, int &freq)const ;

      virtual void startIteration() const {  i=0; }
      virtual bool hasMore() const {  return (i < size()); }
      virtual void nextFreq(lemur::api::DOCID_T &id, int &freq)const ;

      /// return the total number of non-zero elements in the vector
      virtual int size() const {    return (ISet<FreqCount>::size()); }
    protected:
      mutable int i;
    };
  }
}

#endif /* _FREQVECTOR_HPP */
