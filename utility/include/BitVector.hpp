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


#ifndef _BITVECTORH_
#define _BITVECTORH_

#include <stream.h>
#include "common_headers.hpp"

class BitVector : public vector<bool> {
public:
  BitVector() : vector<bool>() {}
  BitVector(size_type n) : vector<bool>(n) {}
  
  void write(ostream &os) const;
  int read(istream &os, int size);
  
private:
  void initBitMask();

private:
  static char bitmask[8];
};

#endif
