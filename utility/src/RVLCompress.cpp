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

#include "RVLCompress.hpp"


// returns number of ints decompressed
int RVLCompress::decompress_ints (unsigned char *data_ptr,
				  int *out_ptr,
				  int num_bytes)
{

  unsigned char *data_end_ptr = data_ptr + num_bytes;
  unsigned char *data_curr_ptr;
  int *out_ptr_end = out_ptr;

  for (data_curr_ptr=data_ptr; data_curr_ptr<data_end_ptr; out_ptr_end++) {
    if (*data_curr_ptr & 128) {
      *out_ptr_end = 127 & *data_curr_ptr;
      data_curr_ptr ++;
    } else if (*(data_curr_ptr+1) & 128) {
      *out_ptr_end = *data_curr_ptr |
	((*(data_curr_ptr + 1) & 127) << 7);
      data_curr_ptr += 2;
    } else if (*(data_curr_ptr+2) & 128) {
      *out_ptr_end = *data_curr_ptr |
	(*(data_curr_ptr + 1) << 7) |
	((*(data_curr_ptr + 2) & 127) << 14);
      data_curr_ptr += 3;
    } else if (*(data_curr_ptr+3) & 128) {
      *out_ptr_end = *data_curr_ptr |
	(*(data_curr_ptr + 1) << 7) |
	(*(data_curr_ptr + 2) << 14) |
	((*(data_curr_ptr + 3) & 127) << 21);
      data_curr_ptr += 4;
    } else {
      *out_ptr_end = *data_curr_ptr |
	(*(data_curr_ptr + 1) << 7) |
	(*(data_curr_ptr + 2) << 14) |
	(*(data_curr_ptr + 3) << 21) |
	((*(data_curr_ptr + 4) & 127) << 28);
      data_curr_ptr += 5;
    }
  } // for
  
  return (out_ptr_end - out_ptr);

}

//return number of bytes in result
int RVLCompress::compress_ints (int *data_ptr,
				unsigned char *out_ptr,
				int size)
{
  int  *data_end_ptr = data_ptr + size;
  int  *data_curr_ptr;
  unsigned int n;
  unsigned char *out_ptr_end = out_ptr;

  for (data_curr_ptr=data_ptr; data_curr_ptr<data_end_ptr; data_curr_ptr++) {
    n = (unsigned int)*data_curr_ptr;

    if (n < pow2_7)
      *out_ptr_end++ = 128 | n;
    else if (n < pow2_14) {
      *out_ptr_end = 127 & n;
      *(out_ptr_end + 1) = 128 | (n >> 7);
      out_ptr_end += 2;
    } else if (n < pow2_21) {
      *out_ptr_end = 127 & n;
      *(out_ptr_end + 1) = 127 & (n >> 7);
      *(out_ptr_end + 2) = 128 | (n >> 14);
      out_ptr_end += 3;
    } else if (n < pow2_28) {
      *out_ptr_end = 127 & n;
      *(out_ptr_end + 1) = 127 & (n >> 7);
      *(out_ptr_end + 2) = 127 & (n >> 14);
      *(out_ptr_end + 3) = 128 | (n >> 21);
      out_ptr_end += 4;
    } else {
      *out_ptr_end = 127 & n;
      *(out_ptr_end + 1) = 127 & (n >> 7);
      *(out_ptr_end + 2) = 127 & (n >> 14);
      *(out_ptr_end + 3) = 127 & (n >> 21);
      *(out_ptr_end + 4) = 128 | (n >> 28);
      out_ptr_end += 5;
      if (n >= pow2_31) {
	cerr << "WARNING: value exceeded int limit in compression" << endl;
      }
    }
  } // for

  return (out_ptr_end - out_ptr);

}


