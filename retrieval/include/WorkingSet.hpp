#ifndef _WORKINGSET_H
#define _WORKINGSET_H

/// Working set for a query (for re-ranking experiment)

/*!

WorkingSet manages a subset of document IDs for a query.
A working set can be used to re-rank documents or for
more efficient scoring.

*/

//---------------------------------------------------------------
// CLASSES: WorkingSet (C. Zhai, 12/21/2000)
//
// WorkingSet: managing the working set for a query
// 
// Possible use: (1) building a working set (adding document IDs)
//               (2) loading a working set (iterating over IDs)
//              
//---------------------------------------------------------------


#include <iostream.h>

class WorkingSet {
public:

  // data types
  typedef int iterator;
  
  // constructors/destructors
  /// construct an empty working set
  WorkingSet(int size = 10000):
    sz(size), actualSz(0), idList(new int[size]) {
  }

  ~WorkingSet() {
    delete [] idList;
  }

  // manipulators
  /// adding a new ID, return true iff full & ID doesn't already exist
  bool addID(int id);
  
  /// load a working set from an istream
  void load(istream &is);

  /// save to an ostream
  void save(ostream &os);

  /// clear all entries
  void clear() { actualSz= 0;}

  // accessors
  iterator begin() { return 0;}
  iterator end() { return actualSz;}
  
  int &operator[](unsigned int idx) const { return idList[idx];}
  
private:
  int sz, actualSz;
  int *idList;
};

// ================= inline implementations ==========================


inline bool  WorkingSet::addID(int id)
{
  int lower=0;
  int upper=actualSz-1;
  int med;

  while (lower <= upper) {
    med = (lower+upper)/2;
    if (id == idList[med]) {
      // found, do nothing
      return false;
    } else if (id > idList[med]) {
      lower = med+1;
    } else { // id < idList[med]
      upper = med-1;
    }
  }
  
  // not found
  if (actualSz >= sz) {
    // full
    return true;
  } else {
    // insert at lower bound position
    for (int i=actualSz; i>lower; i--) {
      idList[i] = idList[i-1];
    }
    idList[lower]=id;
    actualSz++;
    return false;
  }
}

// assuming the following format: totalNumber(n) id1 id2 ... idn
inline void WorkingSet::load(istream &is)
{
  is >> actualSz;
  for (int i=0; i<actualSz; i++) {
    is >> idList[i];
  }
}


inline void WorkingSet::save(ostream &os)
{
  os << actualSz << endl;;
  for (int i=0; i<actualSz; i++) {
    os << idList[i] << endl;;
  }
}


#endif //_WORKINGSET_H


  


