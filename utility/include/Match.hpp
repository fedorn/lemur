/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _MATCH_HPP
#define _MATCH_HPP
///  Byte offsets of a document term (first char to last char inclusive).
class Match {
public:
  /// byte offset of first character of token
  int start;
  /// byte offset of last character of token
  int end;
};
#endif
