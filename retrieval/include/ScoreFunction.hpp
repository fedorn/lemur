
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


#ifndef _SCOREFUNCTION_HPP
#define _SCOREFUNCTION_HPP

#include "TextQueryRep.hpp"
#include "DocumentRep.hpp"

//------------------------------------------------------------
//      Abstract Interface for Retrieval Function 
//------------------------------------------------------------

/// Abstract interface for retrieval function with a default implementation (dot product)

class ScoreFunction {
public:
  /// compute the score contribution of a matched term
  virtual double matchedTermWeight(QueryTerm *qTerm, TextQueryRep *qRep, DocInfo *info, DocumentRep *dRep) { 
    return (qTerm->weight()*dRep->termWeight(qTerm->id(),info));
  }
	    
  /// score adjustment (e.g., appropriate length normalization)
  virtual double adjustedScore(double origScore, TextQueryRep *qRep, DocumentRep *dRep) {
    return origScore; 
  }
};



#endif /* _SCOREFUNCTION_HPP */


