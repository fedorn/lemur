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


#ifndef _LLH_
#define _LLH_

#ifndef NULL
#define NULL ((void *)0)
#endif

template <class Type> class List;

// Internal class for List, do not use as linked list
template <class Type>
class LL {
public:
  // ****************** Constructors ***************************
  // Constructor with data, next, and prev
  LL(Type *d=(Type *) NULL, LL<Type> *n=(LL<Type> *) NULL, LL<Type> *p=(LL<Type> *)NULL);
  LL(LL<Type> &ll) : datamem((Type *)NULL), prevmem((LL<Type> *)NULL), nextmem((LL<Type> *)NULL)
      { *this = ll; }
  // ****************** Destructor ***************************
  ~LL();

  // ****************** Overloaded Operators ******************
  // Copy operator
  LL<Type> &operator=(const LL<Type> &ll);

  // Comparison operators
  int operator!=(const LL<Type> &ll) const { return !(*this == ll); }
  int operator==(const LL<Type> &ll) const;

  // Data accessing operator
  Type *operator[](int num) const;

  // Data search operator
  LL<Type> *operator[](const Type *d);

  // ************************ Member Functions *********************
  void DeleteData();
  Type *Remove();

private:
  // ********************** Member Data ***********************
  Type *datamem;
  LL<Type> *prevmem;
  LL<Type> *nextmem;

  friend class List<Type>;
};


#endif
