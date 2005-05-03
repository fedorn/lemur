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


// -------------------------------------------------------------------
// ISet.hpp (set with index)
// ALB
// Template set class derived from PSet.hpp
//    . retrieve objects in the order in which they were inserted 
//      via operator[int]     
//   . growable
// -------------------------------------------------------------------

#ifndef _ISETH_
#define _ISETH_

#include "PSet.hpp"

namespace lemur
{
  namespace utility
  {
    
    template <class ObjType>
    class ISet : public PSet<ObjType>
    {
    public:
      ISet(): PSet<ObjType>(), index(0) {}
      ISet(const int maxSize_p): index(0) { open(maxSize_p); }
      ~ISet() { close(); }  

      void open(const int maxSize_p) {
        PSet<ObjType>::open(maxSize_p);
        index = new typename PSet<ObjType>::SET_NODE* [this->maxSize+1];
        memset(index, 0, (this->maxSize+1)*sizeof(typename PSet<ObjType>::SET_NODE*));
      }
  
      void close() {
        PSet<ObjType>::close();
        delete [] index; 
        index=0;
      }

      void clear() {
        if (this->maxSize==0) return;
        close();
        open(this->maxSize);
      }

      int size() const { return this->currentSize; }

      int add(const ObjType& u) {
        typename PSet<ObjType>::SET_NODE *sn = PSet<ObjType>::internalAdd(u);
        if (sn==0) return -1;
        index[sn->idx] = sn;
        if (++this->currentSize > this->maxSize) grow((int) (this->currentSize*GROW_FACTOR+1));
        return sn->idx;
      }

      int remove(const ObjType& u) { // remove u from set: returns 1 iff u was in set
        const int idx = internalRemove(u);
        if (idx==-1) return 0;                 // not a member
        this->currentSize--;
        return 1;                              // was a member (not anymore)
      }

      int operator+=(const ObjType& u)  // add an elt to set: returns 1 if added. 
      { return add(u); }
  
      int operator-=(const ObjType& u)// remove elt from set: returns 1 if removed.
      { return remove(u); }

      // NB: When user removes elts. from set, the set is re-indexed, so
      // what is the n'th elt. now may be the n-m'th elt. sometime later
      ObjType& operator[](const int idx) const {   // get n'th elt
        assert(idx<this->currentSize);
        return index[idx]->u; 
      }
  
      int operator[](const ObjType& u) const {    // get idx of u, -1 if not there
        int hashval = computeHash(u);    
        typename PSet<ObjType>::SET_NODE *p = this->hashTable[hashval];
        while(p!=0 && !(p->u==u)) p=p->next;
        return ((p==0)? -1: p->idx);
      }
  
      void grow(const int newSize) {
        this->maxSize = newSize;
        this->hashTableSize = this->smallestPrimeGreaterThan((int) (this->maxSize*SPARSENESS));
        typename PSet<ObjType>::SET_NODE **newIndex = new typename PSet<ObjType>::SET_NODE* [this->maxSize+1];
        typename PSet<ObjType>::SET_NODE **newHashTable = new typename PSet<ObjType>::SET_NODE* [this->hashTableSize];
        memset(newHashTable, 0, this->hashTableSize*sizeof(typename PSet<ObjType>::SET_NODE *));
        for (int i=0; i<this->currentSize; i++) {
          typename PSet<ObjType>::SET_NODE *sn = index[i];
          const int hashval = computeHash(sn->u);
          typename PSet<ObjType>::SET_NODE *snNew = createNode(sn->u);
          snNew->idx = i;
          snNew->next = newHashTable[hashval];
          newHashTable[hashval] = snNew;
          deleteNode(sn);
          newIndex[i] = snNew;
        }
        delete [] index;
        delete [] this->hashTable;
        index = newIndex;
        this->hashTable = newHashTable;
      }
 
    protected:
      int internalRemove(const ObjType &u) {
        // Return idx of removed node. For efficiency, we re-index the set
        // so that what once was the last member now is the idx'th member.
        // (rather than renumbering the entire set starting from idx)
        int idx=PSet<ObjType>::internalRemove(u);
        if (idx==-1) return -1;
        index[idx] = index[this->currentSize-1];
        index[idx]->idx = idx;
        index[this->currentSize-1] = 0;
        return idx;
      }

      int internalRemove(const ObjType &u, const int idx) {
        PSet<ObjType>::internalRemove(u);
        index[idx] = index[this->currentSize-1];
        if (index[idx]->idx != -2) index[idx]->idx=idx;
        index[this->currentSize-1] = 0;
        return idx;
      }

    protected:
      typename PSet<ObjType>::SET_NODE* (*index);              // goes from [dense idx] -> node
    };
  }
}

#endif
