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


#ifndef _COUNTER_HPP
#define _COUNTER_HPP

namespace lemur 
{
  namespace utility
  {
    
    /// Abstract Counter class
    /*!
      The double type, rather than int type, is used so that it may
      be useful in a setting like EM.
      The event index has a range [0,N].
      The caller is responsible for boundary check if necessary. 
    */

    class Counter {
    public:

      virtual ~Counter() {}
      // access functions
  
      /// return the count of a word
      virtual double count(int eventIndex) const= 0;
      /// return the sum of all counts
      virtual double sum() const= 0;
  
      /// iteration over non-zero counts
      virtual void startIteration() const= 0;
      virtual bool hasMore() const= 0;
      virtual void nextCount(int &eventIndex, double &count) const= 0;
    };

    /// Modifiable counter, supports modification of counts
    class ModifiableCounter : public Counter {
    public: 
      // manipulation functions
      virtual ~ModifiableCounter() {}
      virtual void incCount(int eventIndex, double count) = 0;
      virtual void setCount(int eventIndex, double count)  = 0;
    };

    /// Counts stored in an array ( when the element type is int, float/double typed counts will be converted to an integer)

    template<class T>
    class ArrayCounter : public ModifiableCounter {
    public:
  
      ArrayCounter(int size) : sz(size), ct(new T[size]), total(0) {
        for (int i=0; i<size; i++) ct[i]=0;
      }
      virtual ~ArrayCounter() { delete [] ct;}

      /// return the count of an event
      virtual double count(int eventIndex) const{
        return ct[eventIndex];
      }

      /// return the sum of all counts
      virtual double sum() const{
        return total;
      }

      virtual void incCount(int eventIndex, double count) {
        ct[eventIndex] += (T)count;
        total += (T)count;
      }

      virtual void setCount(int eventIndex, double count) {
        total = total - ct[eventIndex]+ (T)count;
        ct[eventIndex] = (T)count;
      }


      virtual void startIteration() const{
        pos=0;
      }

      virtual bool hasMore() const{
        while ((pos < sz) && (ct[pos] == 0))
          pos++;
        return (pos<sz); 
      }

      virtual void nextCount(int &eventIndex, double &count) const{
        eventIndex = pos;
        count = ct[pos];
        pos++;
      }

    protected:
      T *ct;
      int sz;
      T total;
      mutable int pos;
    };
  }
}

#endif /* _COUNTER_HPP */

