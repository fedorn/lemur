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

#ifndef _LEMURMEMPARSER_HPP
#define _LEMURMEMPARSER_HPP

#include "MemParser.hpp"
#include "Index.hpp"

/*! "Parses" a Lemur document loaded into memory.  Provided as a wrapper
 * for the query-based sampling tools.  Uses an existing Lemur index
 * to read to previously parsed document.
 */
class LemurMemParser : public MemParser {
 
public:
  LemurMemParser(Index * ind);
 
  /// Parses a document in memory.
  void parse (doc_t * doc);
  
private:
  Index * index;

};

#endif
