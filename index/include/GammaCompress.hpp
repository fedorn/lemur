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


#ifndef _GAMMACOMPRESSH_
#define _GAMMACOMPRESSH_

/// Gamma compressor

/*!

 GammaCompress

 Implementation of the "gamma" method, as described in
 "Managing Gigabytes" by Witten et al., Chapter 3.
 (The method is originally attributed to P. Elias.)
*/
  
//---------------------------------------------------------------

//  J. Lafferty, 10/29/00
//
//---------------------------------------------------------------

#include "BitVector.hpp"
#include "Compress.hpp"

class GammaCompress : public Compress 
{
public:
  GammaCompress();

public:
  void compress (ostream &os, int n, int * a);
  int decompress (istream &is, int * a);
  int read(istream &is);
  const char * type() { return "gamma"; }

private:
  void initBitMasks ();
  int floorlog2 (double x);
  int gamma_size(int x);
  int gamma_encode(int x, int offset, BitVector & bv);
  int gamma_decode(int * y, int offset, BitVector & bv);

private:
  BitVector bv;
};

#endif
