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

#include "common_headers.hpp"
#include "ScoreAccumulator.hpp"

void ArrayAccumulator::reset()
{
  for (int i=0; i<sz; i++) {
    acc[i]=0;
    status[i]=0;
  }
  p=0;
}

bool ArrayAccumulator::findScore(int id, double &score) const
{ 
  if (status[id-1]>0) {
    score = acc[id-1];
    return true;
  } else {
    return false;
  }
}

bool ArrayAccumulator::hasMore() const {
  if (!(p>=0 && p<=sz)) {
    cerr << "Illegal score" << endl;
    exit(0);
  }
  while (p<sz && status[p]==0) p++;
  return (p<sz);
}

void ArrayAccumulator::nextScore(int &id, double &score) const{
  if (p<sz) id = p+1;
  score = acc[p];
  p++;
}
