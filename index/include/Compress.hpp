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


#ifndef _COMPRESSH_
#define _COMPRESSH_

/// Abstract Compressor

/*!


 Compress

 Abstract interface to the compression routines used for
 inverted lists.  Specification:

   compress(os, n, arr):  compresses the n integers in arr
      and writes to the stream os

   decompress(is, arr): decompresses the bits read from is
      and places them in the integer array arr, returning
      the length of the resulting array
*/
  
//----------------------------------------------------------
//  J. Lafferty, 10/29/00
//
//---------------------------------------------------------------

#include "common_headers.hpp"

class Compress {
public:
  virtual void compress (ostream &os, int n, int * arr)=0;
  virtual int decompress (istream &is, int * arr)=0;
  
  // Read into bit array, do not decompress
  virtual int read(istream &is)=0;
  virtual const char * type ()=0;
};

#endif


  

