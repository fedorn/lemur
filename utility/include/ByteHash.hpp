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


#ifndef _BYTEHASHH_
#define _BYTEHASHH_
namespace lemur
{
  /// Utility and general purpose components.
  namespace utility
  {
    
    unsigned int ByteHash(const char *, const int len, const int seed);
  }
}

#endif
