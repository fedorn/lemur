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


#include "TextQueryRep.hpp"

ArrayQueryRep::ArrayQueryRep(int size, TextQuery &qry, Index &dbIndex):
  ct(new ArrayCounter<double>(size)), scConst(0) 
{
  qry.startTermIteration();
  while (qry.hasMore()) {
    TokenTerm *t = qry.nextTerm();
    int ti = dbIndex.term(t->spelling());
    if (ti>0) {
      incCount(ti, 1);
    } else {
      // cerr << "Unknown query term: "<< t->spelling() << " [ignored]\n";
    }
  }
}

ArrayQueryRep::ArrayQueryRep(int size, FreqVector &qryVec):
  ct(new ArrayCounter<double>(size)), scConst(0) 
{
  qryVec.startIteration();
  while (qryVec.hasMore()) {
    int ti, fq;
    qryVec.nextFreq(ti, fq);
    incCount(ti, fq);
  }
}


QueryTerm *ArrayQueryRep::nextTerm()
{
  int wdIndex;
  double wdCount;
  ct->nextCount(wdIndex, wdCount);
  return (makeQueryTerm(wdIndex, wdCount));
}



bool PseudoFBDocs::hasMore() 
{
  if (howMany>=0 && i >= howMany) {
    return false;
  }
  return (it != res->end());
}

void PseudoFBDocs::nextIDInfo(int &id, double &relProb) 
{
  id = (*it).ind;
  relProb = (*it).val;
  it++;
  i++;
}
