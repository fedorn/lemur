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

#include "Compress.hpp"
#include "BitArray.hpp"

class GammaCompress : public Compress 
{
public:
  GammaCompress();

public:
  void compress (ostream &os, int n, int * a);
  int decompress (istream &is, int * a);
  int read(istream &is, BitArray * ba);
  const char * type() { return "gamma"; }

private:
  void initBitMasks ();
  int floorlog2 (int x);
  int gamma_size(int x);
  int gamma_encode(int x, int offset, BitArray * ba);
  int gamma_decode(int * y, int offset, BitArray * ba);

private:
  BitArray *ba;
};

#endif
