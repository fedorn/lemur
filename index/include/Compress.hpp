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

#include <stdlib.h>
#include <iostream.h>
#include "BitArray.hpp"

class Compress {
public:
  virtual void compress (ostream &os, int n, int * arr)=0;
  virtual int decompress (istream &is, int * arr)=0;
  
  // Read into bit array, do not decompress
  virtual int read(istream &is, BitArray * ba)=0;
  virtual const char * type ()=0;
};

#endif


  

