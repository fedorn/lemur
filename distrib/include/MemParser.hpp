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

#ifndef _MEMPARSER_HPP
#define _MEMPARSER_HPP

#include "Parser.hpp"

/// A document id is a null terminated character array.
typedef char * docid_t;

/// A document is a character array storing the document contents,
/// a length of the array, and a document id.
typedef struct doct {  
  char * doc;
  int len;
  docid_t id;
} doc_t;


/*!
 * Parses a document that is in memory, rather than on disk.
 * Should be paired with a DBManager for query based sampling.
 */
class MemParser : public Parser {
 
public:
 
  /// Parses a document in memory.
  /// This call may be desctructive to the buffer.
  virtual void parse (doc_t * doc) = 0;
  
  virtual void parse (char * filename) { };


};

#endif
