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

#include "DistMergeMethod.hpp"

void lemur::distrib::DistMergeMethod::mergeScoreSet(const lemur::api::IndexedRealVector &indexset, 
                                                    const DocScoreVector* const* scoreset, 
                                                    DocScoreVector &results) {

  const DocScoreVector* docscores;
  DocScoreVector::const_iterator j;
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
