/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/

#ifndef _RVLCOMPRESS_HPP
#define _RVLCOMPRESS_HPP

#include "lemur-platform.h"

#define pow2_7  128
#define pow2_14 16384
#define pow2_21 2097152
#define pow2_28 268435456

#define pow2_31 2147483648U


class RVLCompress {
public:
  ///return number of bytes in result
  static int compress_ints (int *data_ptr, unsigned char *out_ptr, int size);

  /// returns number of ints decompressed
  static int decompress_ints(unsigned char *data_ptr, int *out_ptr, int num_bytes);

  /// size of <data> when compressed with RVLCompress
  static int compressedSize( int data );

  static char* compress_int( char* dest, int data );
  static char* compress_longlong( char* dest, INT64 data );
  static const char* decompress_int( const char* source, int& data );
  static const char* decompress_longlong( const char* source, INT64& data );
  static const char* decompress_int_count( const char* source, int* result, int numInts );
  static const char* skip_ints( const char* source, int numInts );

};

inline const char* RVLCompress::decompress_int( const char* source, int& data ) {
  const unsigned int terminator = (1<<7);
  const unsigned int mask = ((1<<7)-1);

  if( source[0] & terminator ) {
    data = source[0] & mask;
    return source + 1;
  } else if ( source[1] & terminator ) {
    data = (source[0])       | 
           ((source[1]&mask) << 7);
    return source + 2;
  } else if ( source[2] & terminator ) {
    data = (source[0])       | 
           (source[1] << 7)  |
           ((source[2]&mask) << 14);
    return source + 3;
  } else if ( source[3] & terminator ) {
    data = (source[0])       | 
           (source[1] << 7)  |
           (source[2] << 14) |
           ((source[3]&mask) << 21);
    return source + 4;
  } else {
    data = (source[0])       | 
           (source[1] << 7)  |
           (source[2] << 14) |
           (source[3] << 21) |
           ((source[4]&mask) << 28);  
    return source + 5;
  }
}

inline const char* RVLCompress::decompress_longlong( const char* source, INT64& data ) {
  const unsigned int terminator = (1<<7);
  const unsigned int mask = ((1<<7)-1);
  unsigned int i;

  data = 0;

  for( i=0; i<10; i++ ) {
    if( source[i] & terminator ) {
      data |= ((source[i] & mask) << 7*i);
      break;
    } else {
      data |= (source[i] << 7*i);
    }
  }

  return source + i + 1;
}

inline const char* RVLCompress::decompress_int_count( const char* source, int* result, int numInts ) {
  const char* ptr = source;

  for( int i=0; i<numInts; i++ ) {
    ptr = decompress_int( ptr, result[i] );
  }

  return ptr;
}

inline const char* RVLCompress::skip_ints( const char* source, int numInts ) {
  while( numInts-- ) {
    while( !(*source & 0x80) )
      source++;
    source++;
  }
  return source;
}


#endif
