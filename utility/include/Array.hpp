 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _ARRAYH_
#define _ARRAYH_

// 
// Note: define ARRAY_BOUNDARY_CHECK for array boundary checking
// 

// #define ARRAY_BOUNDARY_CHECK
#include <unistd.h>
#include <assert.h>

// basic array class, fixed size, not automatically growable
template <class Type>
class Array {

public:
  // ***************** Constructors ********************
  // Constructor with maxsize parameter
  Array(unsigned int size=0) : realsize(size), maxsize(size)
    { if (size) array = new Type[size]; else array = (Type *) NULL; }

  // Copy method, uses equal operator
  Array(const Array<Type> &a)
    : realsize(a.realsize), maxsize(a.maxsize), array((Type *) NULL)
    { array = new Type[a.realsize]; *this = a; }

  // ***************** Destructor ********************
  ~Array() { if (array) delete [] array; }

  // ***************** operators ********************
  // Equal operator, copies array
  Array<Type> &operator=(const Array<Type> &a)
  {
    if (maxsize < a.maxsize) Grow(a.maxsize,0);
    assert(a.array != NULL);
    for (unsigned int i=0; i<a.maxsize; i++) array[i] = a.array[i];
    Grow(a.maxsize);     // ALB 10/29/97
    return *this;
  }

  Type &operator[](unsigned int idx) const
    {
#ifdef ARRAY_BOUNDARY_CHECK
      assert((idx >= 0) && (idx < maxsize));
#endif
      return array[idx];
    }

  // ***************** member functions ********************
  void Grow(unsigned int newsize, int copy=1) 

{
  if (realsize >= (unsigned int) newsize) { maxsize = newsize; return; }

  Type * newarray;
  if (newsize == 0) {
     newarray = (Type *) NULL;
  }
  else {
     newarray = new Type[newsize];
     assert(newarray != NULL);
  }

  if (realsize)
    {
      if (copy) for (unsigned int i=0; i<maxsize; i++) newarray[i] = array[i];
      delete [] array;
    }

  array = newarray;
  maxsize = newsize;
  realsize = newsize;
}
  Array<Type> Union     (const Array<Type> & B) const 
{
  int length=0;
  int l1=Size();
  int l2=B.Size();   
  Array<Type> C(l1+l2);
  C.Grow(0);

  int i = 0;
  for (i=0; i<l1; i++)
    {
      const Type & var=array[i];
      if (C.Member(var)==0)
        {
          length++;
          C.Grow(length);
          C[length-1]=var;
        }
    }
  for (i=0; i<l2; i++)
    {
      const Type & var=B[i];
      if (C.Member(var)==0)
        {
          length++;
          C.Grow(length);
          C[length-1]=var;
        }
    }
  return C;
}

  Array<Type> Intersect (const Array<Type> & B) const
{
  int length = 0;
  int l1 = Size();
  int l2 = B.Size();
  if (l1 < l2)
  {
    Array<Type> C(l1);
    C.Grow(0);
    for (int i=0; i<l1; i++)
      {
        const Type & var = array[i];
        if ((C.Member(var)==0) && (B.Member(var)==1))
          {
            length++;
            C.Grow(length);
            C[length-1] = var;
          }
      }
    return C;
  }
  else
  {
    Array<Type> C(l2);
    C.Grow(0);
    for (int i=0; i<l2; i++)
      {
        const Type & var = B[i];
        if ((C.Member(var)==0) && (Member(var)==1))
          {
            length++;
            C.Grow(length);
            C[length-1] = var;
         }
      }
      return C;
    }
}

  int Member (const Type & a) const
{
  int l=Size();
  for (int i=0; i<l; i++)
    if (array[i] == a)
      return 1;
  return 0;
}

  const Type *Get() const { return array; }
  Type *Get() { return array; }

  // ***************** accessors ********************
  unsigned int Size() const { return maxsize; }

  public:
    int operator==(const Array<Type> &) const
{
  assert(0);
  /*
  if (Size() != rhs.Size())
    return 0;
  for (unsigned int i=0; i<Size(); i++)
  {
    const Type & var = array[i];
    if (var == rhs[i])   // quick test for sorted arrays
      continue;
    if (! (rhs.Member(var)) || ! (Member(rhs[i])))
	return 0;
  }
  */
  return 1;
}


protected:
  unsigned int RealSize() const { return realsize; }

private:
  unsigned int realsize;	// actually memory allocation  
  unsigned int maxsize;	        // maximum size of array 
  Type *array;	// array of Type

};


#endif
