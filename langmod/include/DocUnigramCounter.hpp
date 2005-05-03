/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _DOCUNIGRAMCOUNTER_HPP
#define _DOCUNIGRAMCOUNTER_HPP

#include "common_headers.hpp"
#include "WeightedIDSet.hpp"
#include "Counter.hpp"
#include "Index.hpp"
namespace lemur 
{
  /// Language model components.
  namespace langmod
  {
    
    /// Counter of unigrams in documents

    class DocUnigramCounter : public lemur::utility::ArrayCounter <int> {
    public:
      /// construct a counter for a doc
      DocUnigramCounter(lemur::api::DOCID_T docID, const lemur::api::Index &homeIndex);

      /// construct a counter for a subset of docs in a collection
      DocUnigramCounter(const vector<lemur::api::DOCID_T> &docSet, const lemur::api::Index &homeIndex);

      /// construct a counter for a subset of weighted docs in a collection
      DocUnigramCounter(const lemur::utility::WeightedIDSet &docSet, const lemur::api::Index &homeIndex);

      /// construct a counter for a whole collection of docs 
      DocUnigramCounter(const lemur::api::Index &collectionIndex);

      virtual ~DocUnigramCounter() {}
      const string lexiconID() const { return (ind.termLexiconID());}

    protected:

      void countDocUnigram(lemur::api::DOCID_T docID, double weight=1);

      const lemur::api::Index &ind;
    };
 
  }
}

#endif /* _DOCUNIGRAMCOUNTER_HPP */
