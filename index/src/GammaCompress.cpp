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
#define BYTESIZE 8
static int bitmask[NUMBITS];

char GammaCompress::bits[BYTESIZE] = 
{
  0x80, 0x40, 0x20, 0x10,
  0x8, 0x4, 0x2, 0x1,
};

int GammaCompress::read_bits(istream & is, vector<int> &bv, int size)
{
  int bytes = size / BYTESIZE;
  if (size % BYTESIZE > 0) bytes++;

  for (int i=0; i<bytes; ++i) {
    char b;
    is.read(&b, sizeof(char));
    for (int i=0; i<BYTESIZE; ++i) {
      if (bits[i] & b) 
	bv.push_back(true);
      else 
	bv.push_back(false);
    }
  }
  return bytes;
}

void GammaCompress::write_bits(ostream & os, vector<int> &bv) 
{
  int size = bv.size();
  int bytes = size / BYTESIZE;
  if (size % BYTESIZE > 0) bytes++;

  int j=0;
  for (int i=0; i<bytes; ++i) {
    char b=0;
    for (int i=0; i<BYTESIZE; ++i) {
      if (j <= size) {
	if (bv[j++] == (int) true) {
	  b = (b ^ bits[i]);
	}
      }
    }
    os.write(&b, sizeof(char));
  }
}

void GammaCompress::initBitMasks() {
  for (int i=0; i<NUMBITS; ++i) {
    bitmask[i] = (1 << i);
  }
}

// x > 0 
int GammaCompress::floorlog2 (double x) {
  static double loge2=log(2.0);
  static double epsilon=1.0e-8;
  return (int) floor((log(x) / loge2)+epsilon);
}

// x > 0
int GammaCompress::gamma_size(int x) {
  return 1 + 2*floorlog2(x);
}

// x > 0
int GammaCompress::gamma_encode(int x, int offset, vector<int> &ba) {
  int l = floorlog2(x);
  int i;
  for (i=0; i<l; ++i) {
    ba.push_back(true);
  }
  ba.push_back(false);
  int z = x - (1 << l);
  for (i=l-1; i>=0; --i) {
    if (bitmask[i]&z) 
      ba.push_back(true);
    else 
      ba.push_back(false);
  }
  return 1+2*l;
}

int GammaCompress::gamma_decode(int * y, int offset, vector<int> &ba) {
  int o=offset;
  int l=0;
  while(ba[o++]) {
    l++;
  }
  int z=0;
  for (int i=l-1; i>=0; --i) {
    if (ba[o++])
      z ^= bitmask[i];
  }
  *y = z + (1<<l);
  return 1+2*l;
}

GammaCompress::GammaCompress() { initBitMasks(); }

void GammaCompress::compress(ostream &os, int n, int * a) {
  int size=0;
  int i;
  for (i=0; i<n; ++i) {
    size += gamma_size(a[i]);
  }
  vector<int> ba;
  int offset=0;
  for (i=0; i<n; ++i) {
    offset += gamma_encode(a[i], offset, ba);
  }
  int s=ba.size();
  os.write((char *)&n, sizeof(int));
  os.write((char *)&s, sizeof(int));
  write_bits(os, ba);
}

int GammaCompress::decompress(istream &is, int * a) {
  int n, size;
  is.read((char *)&n, sizeof(int));
  is.read((char *)&size, sizeof(int));
  if (is.peek() == EOF) return 0;
  vector<int> ca;
  read_bits(is, ca, size);
  int y, offset=0;
  for (int i=0; i<n; ++i) {
    offset += gamma_decode(&y, offset, ca);
    a[i] = y;
  }
  return n;
}

// Read into bit array, do not decompress
int GammaCompress::read(istream &is) {
  int n, size;
  is.read((char *)&n, sizeof(int));
  is.read((char *)&size, sizeof(int));
  if (is.peek() == EOF) return 0;
  bv.reserve(size);
  bv.clear();
  // vector<int> bv;   C.Z. 
  return read_bits(is, bv, size);
}

