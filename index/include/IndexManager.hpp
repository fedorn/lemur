 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _INDEXMANAGER_HPP
#define _INDEXMANAGER_HPP

#include "BasicIndex.hpp"
#include "InvFPIndex.hpp"

/// A group of index management functions
class IndexManager {
public:
  /// An utility function to open an index, automatically recognizing the indexer based on file extension

  static Index *openIndex(const char *indexTOCFile);
};


#endif /* _INDEXMANAGER_HPP */
