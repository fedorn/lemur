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

ArrayQueryRep::ArrayQueryRep(int size, const TermQuery &qry, const Index &dbIndex):
  ct(new ArrayCounter<double>(size)), scConst(0) 
{
  qry.startTermIteration();
  while (qry.hasMore()) {
    const Term *t = qry.nextTerm();
    int ti = dbIndex.term(t->spelling());
    if (ti>0) {
      incCount(ti, 1);
    } else {
      // cerr << "Unknown query term: "<< t->spelling() << " [ignored]\n";
    }
  }
}

ArrayQueryRep::ArrayQueryRep(int size, const FreqVector &qryVec):
  ct(new ArrayCounter<double>(size)), scConst(0) 
{
  qryVec.startIteration();
  while (qryVec.hasMore()) {
    int ti, fq;
    qryVec.nextFreq(ti, fq);
    incCount(ti, fq);
  }
}


QueryTerm *ArrayQueryRep::nextTerm() const 
{
  int wdIndex;
  double wdCount;
  ct->nextCount(wdIndex, wdCount);
  return (makeQueryTerm(wdIndex, wdCount));
}



bool PseudoFBDocs::hasMore() const 
{
  if (howMany>=0 && i >= howMany) {
    return false;
  }
  return (it != res->end());
}

void PseudoFBDocs::nextIDInfo(int &id, double &relProb) const 
{
  i++;
  id = (*it).ind;
  relProb = (noWeight? 1.0 : (*it).val); 
  it++;

}
