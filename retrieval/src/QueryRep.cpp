#include "QueryRep.hpp"

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
      cerr << "Unknown query term :"<< t->spelling() << " [ignored]\n";
    }
  }
}
