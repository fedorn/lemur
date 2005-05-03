/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
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
namespace lemur 
{
  namespace langmod 
  {
    
    /// \brief Unigram counter for documents with in document term probabilities.
    /// Used by the relevance model feedback methods.

    class RelDocUnigramCounter : public lemur::utility::ArrayCounter <double> {
    public:
      /// construct a counter for a doc
      RelDocUnigramCounter(lemur::api::DOCID_T docID, const lemur::api::Index &homeIndex);

      /// construct a counter for a subset of weighted docs in a collection
      RelDocUnigramCounter(const lemur::utility::WeightedIDSet &docSet, const lemur::api::Index &homeIndex);

      virtual ~RelDocUnigramCounter() {}
      const string lexiconID() const { return (ind.termLexiconID());}

    protected:
      /// Fill in the counter values with the probability of each term in
      /// in the document (count/length).
      void countRelDocUnigram(lemur::api::DOCID_T docID, double weight=1);

      const lemur::api::Index &ind;
    };
 
  }
}

#endif /* _RELDOCUNIGRAMCOUNTER_HPP */
