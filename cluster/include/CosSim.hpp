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
  Author: dmf 11/2002
*/

#ifndef _COSSIM_HPP
#define _COSSIM_HPP
#include "SimilarityMethod.hpp"
#include "Index.hpp"
namespace lemur
{
  namespace cluster 
  {
    /// Cosine similarity method for vector space comparisons.
    class CosSim : public SimilarityMethod {
    public:
      /// Precomputes the idf values for the collection.
      CosSim(const lemur::api::Index &dbIndex);
      /// clean up
      virtual ~CosSim();
      /// normalize rep with idf values.
      void weigh(ClusterRep *rep) const {rep->weigh(idfV);}
      /// compute similarity between two reps.
      double similarity(const ClusterRep *vec1, const ClusterRep *vec2) const;
      /// return pointer to precomputed idf values.
      const double *idfValues() const {return idfV;}
    
    protected:
      const lemur::api::Index &ind;
      double *idfV;
      int numDocs, numTerms;
    };
  }
}

#endif /* _COSSIM_HPP */
