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


#ifndef _DOCUNIGRAMCOUNTER_HPP
#define _DOCUNIGRAMCOUNTER_HPP

#include "common_headers.hpp"
#include "WeightedIDSet.hpp"
#include "Counter.hpp"
#include "Index.hpp"

/// Counter of unigrams in documents

class DocUnigramCounter : public ArrayCounter <int> {
public:
  /// construct a counter for a doc
  DocUnigramCounter(int docID, const Index &homeIndex);

  /// construct a counter for a subset of docs in a collection
  DocUnigramCounter(const vector<int> &docSet, const Index &homeIndex);

  /// construct a counter for a subset of weighted docs in a collection
  DocUnigramCounter(const WeightedIDSet &docSet, const Index &homeIndex);

  /// construct a counter for a whole collection of docs 
  DocUnigramCounter(const Index &collectionIndex);

  virtual ~DocUnigramCounter() {}
  const string lexiconID() const { return (ind.termLexiconID());}

protected:

  void countDocUnigram(int docID, double weight=1);

  const Index &ind;
};


#endif /* _DOCUNIGRAMCOUNTER_HPP */
