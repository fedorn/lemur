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



#include <cmath>
#include <cstdlib>
#include "GammaCompress.hpp"

#define NUMBITS 32
static int bitmask[NUMBITS];

void GammaCompress::initBitMasks() {
  for (int i=0; i<NUMBITS; ++i) {
    bitmask[i] = (1 << i);
  }
}

// x > 0 
int GammaCompress::floorlog2 (int x) {
  static double loge2=log(2);
  return (int) floor((log(x) / loge2));
}

// x > 0
int GammaCompress::gamma_size(int x) {
  return 1 + 2*floorlog2(x);
}

// x > 0
int GammaCompress::gamma_encode(int x, int offset, BitArray * ba) {
  int o=offset;
  int l = floorlog2(x);
  for (int i=0; i<l; ++i) {
    ba->Sett(o++);
  }
  ba->Clear(o++);
  int z = x - (1 << l);
  for (int i=l-1; i>=0; --i) {
    if (bitmask[i]&z) 
      ba->Sett(o++);
    else 
      ba->Clear(o++);
  }
  return 1+2*l;
}

int GammaCompress::gamma_decode(int * y, int offset, BitArray * ba) {
  int o=offset;
  int l=0;
  while(ba->Get(o++)) {
    l++;
  }
  int z=0;
  for (int i=l-1; i>=0; --i) {
    if (ba->Get(o++))
      z ^= bitmask[i];
  }
  *y = z + (1<<l);
  return 1+2*l;
}

GammaCompress::GammaCompress() : ba(NULL) { initBitMasks(); }

void GammaCompress::compress(ostream &os, int n, int * a) {
  int size=0;
  for (int i=0; i<n; ++i) {
    size += gamma_size(a[i]);
  }
  BitArray * ba = new BitArray(size);
  int offset=0;
  for (int i=0; i<n; ++i) {
    offset += gamma_encode(a[i], offset, ba);
  }
  int s=ba->Size();
  os.write((char *)&n, sizeof(int));
  os.write((char *)&s, sizeof(int));
  ba->Write(os);
  delete ba;
}

int GammaCompress::decompress(istream &is, int * a) {
  int n, size;
  is.read((char *)&n, sizeof(int));
  is.read((char *)&size, sizeof(int));
  if (is.peek() == EOF) return 0;
  BitArray * ca = new BitArray(size);
  ca->Read(is);
  int y, offset=0;
  for (int i=0; i<n; ++i) {
    offset += gamma_decode(&y, offset, ca);
    a[i] = y;
  }
  delete ca;
  return n;
}

// Read into bit array, do not decompress
int GammaCompress::read(istream &is, BitArray * ba) {
  int n, size;
  is.read((char *)&n, sizeof(int));
  is.read((char *)&size, sizeof(int));
  if (is.peek() == EOF) return 0;
  ba->Grow(size, 0);
  ba->Read(is);
  return n;
}

