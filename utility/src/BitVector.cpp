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


#include <math.h>
#include "BitVector.hpp"

#define BYTESIZE 8

char BitVector::bitmask[BYTESIZE] = 
{
  0x80, 0x40, 0x20, 0x10,
  0x8, 0x4, 0x2, 0x1,
};

int BitVector::read(istream & is, int size)
{
  int bytes = size / BYTESIZE;
  if (size % BYTESIZE > 0) bytes++;
  BitVector &bv = *this;

  int j=0;
  for (int i=0; i<bytes; ++i) {
    char b;
    is.read(&b, sizeof(char));
    for (int i=0; i<BYTESIZE; ++i) {
      if (bitmask[i] & b) 
	bv[j++] = true;
      else 
	bv[j++] = false;
    }
  }
  return bytes;
}

void BitVector::write(ostream & os) const
{
  const BitVector &bv = *this;
  int size = bv.size();
  int bytes = size / BYTESIZE;
  if (size % BYTESIZE > 0) bytes++;

  int j=0;
  for (int i=0; i<bytes; ++i) {
    char b=0;
    for (int i=0; i<BYTESIZE; ++i) {
      if (j <= size) {
	if (bv[j++] == true) {
	  b = (b ^ bitmask[i]);
	}
      }
    }
    os.write(&b, sizeof(char));
  }
}

