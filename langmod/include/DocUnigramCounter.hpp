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

#ifndef _DOCUNIGRAMCOUNTER_HPP
#define _DOCUNIGRAMCOUNTER_HPP

#include <vector.h>
#include "WeightedIDSet.hpp"
#include "Counter.hpp"
#include "Index.hpp"

/// Counter of unigrams in documents

class DocUnigramCounter : public ArrayCounter <int> {
public:
  /// construct a counter for a doc
  DocUnigramCounter(int docID, Index &homeIndex);

  /// construct a counter for a subset of docs in a collection
  DocUnigramCounter(vector<int> &docSet, Index &homeIndex);

  /// construct a counter for a subset of weighted docs in a collection
  DocUnigramCounter(WeightedIDSet &docSet, Index &homeIndex);

  /// construct a counter for a whole collection of docs 
  DocUnigramCounter(Index &collectionIndex);

  virtual ~DocUnigramCounter() {}
  const char *lexiconID() { return (ind.termLexiconID());}

protected:

  void countDocUnigram(int docID, double weight=1);

  Index &ind;
};


#endif /* _DOCUNIGRAMCOUNTER_HPP */
