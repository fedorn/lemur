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

#ifndef _SIMILARITYMETHOD_HPP
#define _SIMILARITYMETHOD_HPP
#include "ClusterRep.hpp"
namespace lemur 
{
  namespace cluster 
  {
    /// Generic similarity method.
    class SimilarityMethod {
    public:
      SimilarityMethod() {}
      virtual ~SimilarityMethod() {}
      /// Normalize the rep
      virtual void weigh(ClusterRep *vec) const {}
      /// score two vectors.
      virtual double similarity(const ClusterRep *vec1, 
                                const ClusterRep *vec2) const = 0;
    };
  }
}

#endif /* _SIMILARITYMETHOD_HPP */
