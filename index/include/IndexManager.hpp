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



#ifndef _INDEXMANAGER_HPP
#define _INDEXMANAGER_HPP
#include "Index.hpp"
// force parsing includes to get dependencies.
#include "indri/Repository.hpp"
/// A group of index management functions
class IndexManager {
public:
  /// An utility function to open an index, automatically recognizing the indexer based on file extension. Caller must delete the Index instance when finishing using it.

  static Index *openIndex(const string &indexTOCFile);
};


#endif /* _INDEXMANAGER_HPP */
