/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _MATCH_HPP
#define _MATCH_HPP
#include "IndexTypes.hpp"
namespace lemur 
{
  namespace parse
  {
    
    ///  Byte offsets of a document term (first char to last char inclusive).
    class Match {
    public:
      /// byte offset of first character of token
      int start;
      /// byte offset of last character of token
      int end;
      /// token position
      lemur::api::LOC_T pos;
      /// term ID
      lemur::api::TERMID_T termid;
    };
  }
}

#endif
