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

#include "DistMergeMethod.hpp"

void DistMergeMethod::mergeScoreSet(IndexedRealVector &indexset, DocScoreVector** scoreset, DocScoreVector &results) {

  DocScoreVector* docscores;
  DocScoreVector::iterator j;
  double indexscore;
  for (int i=0;i<indexset.size();i++) {
    docscores = scoreset[i];
    indexscore = indexset[i].val;
    for (j=docscores->begin(); j!= docscores->end();j++) {
      results.PushValue((*j).id, score(indexscore, (*j).val));
    }
  }
  results.Sort();
}