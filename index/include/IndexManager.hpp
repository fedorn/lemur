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
