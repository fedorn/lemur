/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

/*
  author: fff
*/
#ifndef _RELDOCUNIGRAMCOUNTER_HPP
#define _RELDOCUNIGRAMCOUNTER_HPP

#include "common_headers.hpp"
#include "WeightedIDSet.hpp"
#include "Counter.hpp"
#include "Index.hpp"
#include <cmath>

/// Unigram counter for documents with in document term probabilities.
/// Used by the relevance model feedback methods.

class RelDocUnigramCounter : public ArrayCounter <double> {
public:
  /// construct a counter for a doc
  RelDocUnigramCounter(DOCID_T docID, const Index &homeIndex);

  /// construct a counter for a subset of weighted docs in a collection
  RelDocUnigramCounter(const WeightedIDSet &docSet, const Index &homeIndex);

  virtual ~RelDocUnigramCounter() {}
  const string lexiconID() const { return (ind.termLexiconID());}

protected:
  /// Fill in the counter values with the probability of each term in
  /// in the document (count/length).
  void countRelDocUnigram(DOCID_T docID, double weight=1);

  const Index &ind;
};


#endif /* _RELDOCUNIGRAMCOUNTER_HPP */
