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
// PSet.hpp (Plain set)
// ALB
//
// Template set class (base class of family of set classes)
//    . add objects using operator+() or add()
//    . remove objects using operator-() or remove()
//    . query for the presence of an object in set via operator[object]
//    . if user specifies no removals will occur, uses (efficient) block memory
//
// Required member functions of template ObjType:
//     hash(): returns a 32-bit hash of object
//         ==: returns 1 if objects are equal
//          =: assignment operator
// 
// Note also: ISet (set with index)
// -------------------------------------------------------------------

#ifndef _PSETH_
#define _PSETH_

#include "common_headers.hpp"
#include <cmath>
#include <memory.h>
#include <cassert>
namespace lemur 
{
  namespace utility 
  {
    
    static const float SPARSENESS = 1.5;
    static const float GROW_FACTOR = 2.0;


    template <class ObjType>
    class PSet
    {
    protected:
      struct SET_NODE {
        ObjType          u;
        int              idx;           // used in derived classes, but not here
        struct SET_NODE *next;
      } ;
    
    public:
      PSet(): currentSize(0),maxSize(0),hashTable(0) {}
      PSet(const int maxSize_p) : currentSize(0),hashTable(0) { open(maxSize_p); }
      ~PSet() { close(); }
  
      const int size()    const { return currentSize; }
      const int maxsize() const { return maxSize;     }

      void open(const int maxSize_p) {
        assert(hashTable==0);   // cannot call open() twice!
        assert(maxSize_p>0); 
        maxSize = maxSize_p;
        if (maxSize<2) maxSize=2;
        hashTableSize = smallestPrimeGreaterThan((int) (maxSize*SPARSENESS));
        hashTable = new SET_NODE * [hashTableSize];
        memset(hashTable, 0, hashTableSize*sizeof(SET_NODE *));
        setNodeSize = sizeof(SET_NODE);
      }
  
      void close() {
        if (hashTable) {
          for (int i=0; i<hashTableSize; i++) {
            SET_NODE *node=hashTable[i]; 
            while (node) {
              SET_NODE *next = node->next; 
              delete node;
              node = next;
            }
          }
          delete [] hashTable; 
        }
        hashTable=0;
        currentSize=0;
      }
  
      void clear() {
        if (maxSize==0) return;
        close();
        open(maxSize);
      }
  
      int add(const ObjType& u) {
        SET_NODE *sn = internalAdd(u);
        if (sn==0) return -1;                       // already a member
        assert(currentSize++<=maxSize);   
        return sn->idx;                            // new member
      }

      int remove(const ObjType& u) { // remove u from set: returns 1 iff u was in set
        const int idx = internalRemove(u);
        if (idx==-1) return 0;                 // not a member
        currentSize--;
        return 1;                              // was a member (not anymore)
      }
  
      int operator+=(const ObjType& u)  // add an elt to set: returns 1 if added. 
      { return add(u); }
  
      int operator-=(const ObjType& u)// remove elt from set: returns 1 if removed.
      { return remove(u); }
  
      const ObjType &operator[](const int idx) const {
        int a;
        a=idx; // suppress compiler warnings about not using idx
        cerr <<"PSet: operator[] (int) not supported!"<<endl;
        abort();
      }

      int operator[] (const ObjType& u) const { // returns +1 if found in set, -1 o/w
        int hashval = computeHash(u);
        SET_NODE *p = hashTable[hashval];
        while(p!=0 && !(p->u==u)) p=p->next;
        return ((p==0)? -1: 1);
      }
  

    protected:
      SET_NODE *internalAdd(const ObjType &u) {
        int hashval = computeHash(u);

        // look for this object in the appropriate linked list
        for (SET_NODE* p=hashTable[hashval]; p!=0; p=p->next)
          if (p->u==u) return 0;                 // already a member
    
        // create new node and stick at head of linked list
        SET_NODE *sn = createNode(u);
        sn->idx  = currentSize;
        sn->next = hashTable[hashval];
        hashTable[hashval] = sn;    
        return sn;
      }
  
      int internalRemove(const ObjType &u) {
        // returns index of extracted object, or -1 if not there.
        int hashval = computeHash(u);
        SET_NODE* sn = hashTable[hashval];
        SET_NODE** pLast = &(hashTable[hashval]);
        while (sn) {
          if (sn->u == u) {
            int idx = sn->idx;
            *pLast = sn->next;
            deleteNode(sn);
            return idx;
          }
          pLast = &(sn->next);
          sn = sn->next;
        }
        return -1;
      }
  
      long smallestPrimeGreaterThan(const int n) {
        for (int i=n+1;; i++) {   
          // look for a j<sqrt(i) which divides evenly into i
          int s = (int) sqrt((float) i);
          int j;
          for (j=2; j<=s; j++)
            if ((i % j)==0) break; 
          if (j>s) return i;
        }
      }
  
    protected:
      int computeHash(const ObjType &u) const {
        int h = u.hash() % hashTableSize;
        if (h<0) h*=-1;
        return h;
      }
  
      // memory management of nodes
      SET_NODE *createNode(const ObjType &u) {
        SET_NODE *n = new SET_NODE;
        n->u = u;              
        return n;
      }
  
      void deleteNode(SET_NODE *node) { delete node; }
  
    protected:
      int currentSize;                    // # of elts currently in set 
      int maxSize;                        // # of elts allowable in set 
      int hashTableSize;              // size of hash table containing set  
      SET_NODE** hashTable;
      int setNodeSize;
    };
  }
}

#endif



