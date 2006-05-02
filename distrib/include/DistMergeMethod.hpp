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

#ifndef _DISTMERGEMETHOD_HPP
#define _DISTMERGEMETHOD_HPP


#include "Param.hpp"
#include "common_headers.hpp"
#include "IndexedReal.hpp"
#include "DocScore.hpp"

#define  CORI_MERGE 0
#define  SINGLETYPEREGR_MERGE 1
#define  MULTITYPEREGR_MERGE 2
/// Distributed retrieval merge method parameters.
namespace DistMergeMethodParameter {

  /// the result merging method
  static int mergeMethod;

  static void get() {
    mergeMethod=lemur::api::ParamGetInt("mergeMethod",0);
  }
}


namespace lemur 
{
  namespace distrib
  {
    /// \brief Abstract interface for distributed retrieval merging of scores.
    /*! 
      Merge scores from individual databases.
      See RetrievalMethod for database ranking algorithms (CORIRetMethod).
      See DistSearchMethod for multiple individual database retrieval.

    **/

    class DistMergeMethod {
    public:
      virtual ~DistMergeMethod() {};

      /// merge a set of scores that were obtained from retrieval on the individual databases in the indexset.  return the merged results into a DocScoreVector.  The scores in scoreset should correlate with the indexes in indexset.  
      virtual void mergeScoreSet(const lemur::api::IndexedRealVector &indexset, 
                                 const DocScoreVector* const* scoreset, 
                                 DocScoreVector &results);

    protected:
      /// create a score for the merge based on the index ranking score and the document score 
      virtual double score(double dbscore, double docscore) const =0;
    };
  }
}

#endif
