 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
