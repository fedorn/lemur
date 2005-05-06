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


#include "TextQueryRep.hpp"

using namespace lemur::api;

lemur::retrieval::ArrayQueryRep::ArrayQueryRep(int size, const TermQuery &qry, const Index &dbIndex):
  ct(new lemur::utility::ArrayCounter<double>(size)), scConst(0) 
{
  qry.startTermIteration();
  while (qry.hasMore()) {
    const Term *t = qry.nextTerm();
    TERMID_T ti = dbIndex.term(t->spelling());
    if (ti>0) {
      incCount(ti, 1);
    } else {
      // cerr << "Unknown query term: "<< t->spelling() << " [ignored]\n";
    }
  }
}

lemur::retrieval::ArrayQueryRep::ArrayQueryRep(int size, const lemur::utility::FreqVector &qryVec):
  ct(new lemur::utility::ArrayCounter<double>(size)), scConst(0) 
{
  qryVec.startIteration();
  while (qryVec.hasMore()) {
    TERMID_T ti;
    int fq;
    qryVec.nextFreq(ti, fq);
    incCount(ti, fq);
  }
}


lemur::api::QueryTerm *lemur::retrieval::ArrayQueryRep::nextTerm() const 
{
  int wdIndex;
  double wdCount;
  ct->nextCount(wdIndex, wdCount);
  return (makeQueryTerm(wdIndex, wdCount));
}



bool lemur::api::PseudoFBDocs::hasMore() const 
{
  if (howMany>=0 && i >= howMany) {
    return false;
  }
  return (it != res->end());
}

void lemur::api::PseudoFBDocs::nextIDInfo(int &id, double &relProb) const 
{
  i++;
  id = (*it).ind;
  relProb = (noWeight? 1.0 : (*it).val); 
  it++;

}
