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



// ------------------------------------------------------------------
// CSet.H (Set with counts)
// ALB
// derived from ISet.H (set with index)
// ---------------------------------------------------------------

#ifndef _CSETH_
#define _CSETH_

// #pragma interface

#include <ISet.hpp>
namespace lemur 
{
  namespace utility
  {
    
    template <class ObjType, class CountType>
    class CSet : public ISet<ObjType>
    {
    public:
      CSet() :  ISet<ObjType>(), countOfIndex(0) {}
      CSet(const int maxSize_) : countOfIndex(0) { open(maxSize_); }
      ~CSet() { close(); }

      void open(const int maxSize_) {
        ISet<ObjType>::open(maxSize_); 
        countOfIndex = new CountType [maxSize_ + 1];
        memset(countOfIndex, 0, (1+maxSize_) * sizeof(CountType));
      }
  
      void close() {
        ISet<ObjType>::close();
        delete [] countOfIndex;
        countOfIndex=0;
      }

      void clear() {
        if (this->maxSize==0) return;
        close();
        open(this->maxSize);
      }

      int add(const ObjType& u, const CountType &count=(CountType) 1.0) {
        // add u <count> times to set; return idx of u if new, -1 if old
        typename PSet<ObjType>::SET_NODE  *sn;
        sn = PSet<ObjType>::internalAdd(u);
        if (sn==0) {
          // already in set, but need to increment count anyway
          int idx = operator[](u);
          countOfIndex[idx] += count;
          return -1;
        }
        this->index[sn->idx] = sn;
        countOfIndex[sn->idx] = count;
        if (++this->currentSize>this->maxSize) 
          grow((int) (this->currentSize*GROW_FACTOR + 1));
        return sn->idx;
      }
  
      int remove(const ObjType& u){
        // remove u from set completely: returns 1 iff u was in set
        const int idx = ISet<ObjType>::internalRemove(u);
        if (idx==-1) return 0;                 // not a member
        countOfIndex[idx] = countOfIndex[this->currentSize-1];
        countOfIndex[this->currentSize-1] = 0;
        this->currentSize--;
        return 1;                              // was a member (not anymore)
      }

      int operator+=(const ObjType& u)  // add an elt to set: returns 1 if added. 
      { return add(u); }
  
      int operator-=(const ObjType& u)// remove elt from set: returns 1 if removed.
      { return remove(u); }

      const CountType count(const ObjType& u) const {
        int idx=ISet<ObjType>::operator[](u); 
        if (idx==-1) return 0;
        return count(idx);
      }
  
      const CountType count(const int idx) const { return countOfIndex[idx]; }

      void setCount(const ObjType&u, const CountType count) {
        int idx=ISet<ObjType>::operator[](u); 
        assert(idx!=-1);
        setCount(idx,count);
      }
  
      void setCount(const int idx,const CountType count) {countOfIndex[idx]=count;}
  
      void grow(const int newSize) {
        ISet<ObjType>::grow(newSize);
        CountType *newCountOfIndex = new CountType [this->maxSize+1];
        memcpy(newCountOfIndex, countOfIndex, this->currentSize*sizeof(CountType));
        delete  [] countOfIndex;
        countOfIndex = newCountOfIndex;
      }
  
      // alternate interface
  
    public: 
      void ClearData() { clear(); }
  
    protected:
      CountType *countOfIndex;
    };
  }
}

#endif

