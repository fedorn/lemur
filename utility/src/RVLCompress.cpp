/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

/*
  10/18/2002 -- dmf Remove warning value exceeded int limit in compression
  from compress_ints because it is exercised to often with the termInfoList
  compression.
 */
#include "RVLCompress.hpp"

#define COMPRESS_MASK                 ((1<<7)-1)
#define COMPRESS_TERMINATE_BIT        (1<<7)
#define COMPRESS_BYTE( d, in, b )     d[b] = (char) ((in >> 7*b) & ((1<<7)-1))
#define COMPRESS_TERMINATE( d, b )    d[b] = d[b] | COMPRESS_TERMINATE_BIT

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
      #if 0
      if (n >= pow2_31) {
	cerr << "WARNING: value exceeded int limit in compression" << endl;
      }
      #endif
    }
  } // for

  return (out_ptr_end - out_ptr);

}

int RVLCompress::compressedSize( int data ) {
  if( data < pow2_7 ) {
    return 1;
  } else if ( data < pow2_14 ) {
    return 2;
  } else if ( data < pow2_21 ) {
    return 3;
  } else if ( data < pow2_28 ) {
    return 4;
  } else {
    return 5;
  }
}

char* RVLCompress::compress_int( char* dest, int data ) {
  if( data < (1<<7) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_TERMINATE( dest, 0 );
    return dest + 1;
  } else if( data < (1<<14) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_TERMINATE( dest, 1 );
    return dest + 2;
  } else if( data < (1<<21) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_TERMINATE( dest, 2 );
    return dest + 3;
  } else if( data < (1<<28) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_TERMINATE( dest, 3 );
    return dest + 4;
  } else {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_BYTE( dest, data, 4 );
    COMPRESS_TERMINATE( dest, 4 );
    return dest + 5;
  }
}


char* RVLCompress::compress_longlong( char* dest, INT64 id ) {
  UINT64 data = UINT64(id);

  if( data < (UINT64(1)<<7) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_TERMINATE( dest, 0 );
    return dest + 1;
  } else if( data < (UINT64(1)<<14) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_TERMINATE( dest, 1 );
    return dest + 2;
  } else if( data < (UINT64(1)<<21) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_TERMINATE( dest, 2 );
    return dest + 3;
  } else if( data < (UINT64(1)<<28) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_TERMINATE( dest, 3 );
    return dest + 4;
  } else if( data < (UINT64(1)<<35) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_BYTE( dest, data, 4 );
    COMPRESS_TERMINATE( dest, 4 );
    return dest + 5;
  } else if( data < (UINT64(1)<<42) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_BYTE( dest, data, 4 );
    COMPRESS_BYTE( dest, data, 5 );
    COMPRESS_TERMINATE( dest, 5 );
    return dest + 6;
  } else if( data < (UINT64(1)<<49) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_BYTE( dest, data, 4 );
    COMPRESS_BYTE( dest, data, 5 );
    COMPRESS_BYTE( dest, data, 6 );
    COMPRESS_TERMINATE( dest, 6 );
    return dest + 7;
  } else if( data < (UINT64(1)<<56) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_BYTE( dest, data, 4 );
    COMPRESS_BYTE( dest, data, 5 );
    COMPRESS_BYTE( dest, data, 6 );
    COMPRESS_BYTE( dest, data, 7 );
    COMPRESS_TERMINATE( dest, 7 );
    return dest + 8;
  } else if( data < (UINT64(1)<<63) ) {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_BYTE( dest, data, 4 );
    COMPRESS_BYTE( dest, data, 5 );
    COMPRESS_BYTE( dest, data, 6 );
    COMPRESS_BYTE( dest, data, 7 );
    COMPRESS_BYTE( dest, data, 8 );
    COMPRESS_TERMINATE( dest, 8 );
    return dest + 9;
  } else {
    COMPRESS_BYTE( dest, data, 0 );
    COMPRESS_BYTE( dest, data, 1 );
    COMPRESS_BYTE( dest, data, 2 );
    COMPRESS_BYTE( dest, data, 3 );
    COMPRESS_BYTE( dest, data, 4 );
    COMPRESS_BYTE( dest, data, 5 );
    COMPRESS_BYTE( dest, data, 6 );
    COMPRESS_BYTE( dest, data, 7 );
    COMPRESS_BYTE( dest, data, 8 );
    COMPRESS_BYTE( dest, data, 9 );
    COMPRESS_TERMINATE( dest, 9 );
    return dest + 10;
  }
}
