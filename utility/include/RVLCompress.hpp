/*==========================================================================
 * Copyright (c) 2002 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _RVLCOMPRESS_HPP
#define _RVLCOMPRESS_HPP

#include "common_headers.hpp"

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

};

#endif
