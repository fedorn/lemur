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

#ifndef _SCOREFUNCTION_HPP
#define _SCOREFUNCTION_HPP

#include "QueryRep.hpp"
#include "DocumentRep.hpp"

//------------------------------------------------------------
//      Abstract Interface for Retrieval Function 
//------------------------------------------------------------

/// Abstract interface for retrieval function with a default implementation (dot product)

class ScoreFunction {
public:
  /// compute the score contribution of a matched term
  virtual double matchedTermWeight(QueryTerm *qTerm, QueryRep *qRep, DocInfo *info, DocumentRep *dRep) { 
    return (qTerm->weight()*dRep->termWeight(qTerm->id(),info));
  }
	    
  /// score adjustment (e.g., appropriate length normalization)
  virtual double adjustedScore(double origScore, QueryRep *qRep, DocumentRep *dRep) {
    return origScore; 
  }
};



#endif /* _SCOREFUNCTION_HPP */


