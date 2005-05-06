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

#include "common_headers.hpp"
#include "ScoreAccumulator.hpp"

void lemur::retrieval::ArrayAccumulator::reset()
{
  for (int i=0; i<sz; i++) {
    acc[i]=0;
    status[i]=0;
  }
  p=0;
}

bool lemur::retrieval::ArrayAccumulator::findScore(int id, double &score) const
{ 
  if (status[id-1]>0) {
    score = acc[id-1];
    return true;
  } else {
    return false;
  }
}

bool lemur::retrieval::ArrayAccumulator::hasMore() const {
  if (!(p>=0 && p<=sz)) {
    cerr << "Illegal score" << endl;
    exit(0);
  }
  while (p<sz && status[p]==0) p++;
  return (p<sz);
}

void lemur::retrieval::ArrayAccumulator::nextScore(int &id, double &score) const{
  if (p<sz) id = p+1;
  score = acc[p];
  p++;
}
