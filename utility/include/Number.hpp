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

// ----------------------------------------------------------------------
// Number.hpp
// 10.5.95 
// ----------------------------------------------------------------------

#ifndef _NUMBERH_
#define _NUMBERH_

#pragma interface

#include <iostream.h>
#include <fstream.h>

template <class U>
class Number
{
public:
  Number() {}
  Number(const U& u_p) { u = u_p; }

  int hash() const 
    {
	int i, hashval=1, nBytes=sizeof(u);
	char *p = (char*) &u;
	for (i=0; i<nBytes; i++)
	  hashval = p[i] + 31*hashval;
	if (hashval<0) hashval*=-1;
	return hashval;
    }
  int Hash() const { return hash(); }

  void read(istream &is)  { is.read(&u, sizeof(u));  }
  void write(ostream &os)  { os.write(&u, sizeof(u));  }

  int operator<(const Number<U>& that)  const { return (u< that.u? 1: 0); }
  int operator<=(const Number<U>& that) const { return (u<=that.u? 1: 0); }
  int operator>(const Number<U>& that)  const { return (u> that.u? 1: 0); }
  int operator>=(const Number<U>& that) const { return (u>=that.u? 1: 0); }
  int operator==(const Number<U>& that) const { return (u==that.u? 1: 0); }
  int operator!=(const Number<U>& that) const { return (u!=that.u? 1: 0); }

  Number<U> operator+=(const Number<U>& that) { u+=that.u; return u; }
  Number<U> operator-=(const Number<U>& that) { u-=that.u; return u; }
  Number<U> operator*=(const Number<U>& that) { u*=that.u; return u; }
  Number<U> operator/=(const Number<U>& that) { u/=that.u; return u; }

  Number<U> operator+(const Number<U>& that)  { return u+that.u; }
  Number<U> operator-(const Number<U>& that)  { return u-that.u; }
  Number<U> operator*(const Number<U>& that)  { return u*that.u; }
  Number<U> operator/(const Number<U>& that)  { return u/that.u; }


  template <U> friend ostream& operator<<(ostream&, const Number<U>&) {
    os << s.u; return os;}
  // changed to remove dependence on GUIDE_DECLS, C. Zhai, 2/20/2001
  
  // friend ostream& operator<<(ostream&, const Number<U>&);
  // this old version didn't work! (C. Zhai)

  operator U() const { return u; }
  
private:
  U u;
};

// the following doesn't seem to work

/* It's now inside the class declaration. C. Zhai, 2/20/2001
template <class U>
ostream& operator<<(ostream& os,  const Number<U>& s) {os<<s.u;return os;}
*/

#endif

