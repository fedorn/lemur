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

#ifndef _DISTMERGEMETHOD_HPP
#define _DISTMERGEMETHOD_HPP

/** 
    Abstract interface for distributed retrieval merging of scores.  
    Merge scores from individual databases.
    See RetrievalMethod for database ranking algorithms (CORIRetMethod).
    See DistSearchMethod for multiple individual database retrieval.

 **/

#include "common_headers.hpp"
#include "IndexedReal.hpp"
#include "DocScore.hpp"

class DistMergeMethod {
public:
  virtual ~DistMergeMethod() {};

  /// merge a set of scores that were obtained from retrieval on the individual databases in the indexset.  return the merged results into a DocScoreVector.  The scores in scoreset should correlate with the indexes in indexset.  
  virtual void mergeScoreSet(IndexedRealVector &indexset, DocScoreVector** scoreset, DocScoreVector &results);

protected:
  /// create a score for the merge based on the index ranking score and the document score 
  virtual double score(double dbscore, double docscore)=0;
};

#endif
