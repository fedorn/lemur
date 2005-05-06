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

#ifndef _CORIMERGEMETHOD_HPP
#define _CORIMERGEMETHOD_HPP


#include "DistMergeMethod.hpp"
namespace lemur
{
  /// Distributed retrieval components.
  namespace distrib
  {
    

    /// \brief Implementation of CORI merging  method. 
    /*! 
      Merge scores from individual 
      databases, assuming that the scores we receive have already been 
      normalized for this merge algorithm.
      See RetrievalMethod for database ranking algorithms (CORIRetMethod).
      See DistSearchMethod for multiple individual database retrieval.

    **/
    class CORIMergeMethod: public DistMergeMethod {
    public:
      CORIMergeMethod(double constA=0.4, double constB=1.4);
      ~CORIMergeMethod();

    protected:
      /// create a score for the merge based on the index ranking score and the document score 
      double score(double dbscore, double docscore) const;

    private:
      double A;    // a constant for the formula. default is 0.4
      double B;    // a constant for the formula. default is 1.4
    };
  }
}

#endif
