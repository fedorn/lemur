
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
//      Abstract Interface for Inverted index based Retrieval Function 
//------------------------------------------------------------

/// Abstract interface for retrieval function with a default implementation (dot product)

/*!
  This is an abstract interface for a retrieval function that can be written
 as involving a sum over all the matched terms between a query and a document.
Many retrieval functions can be rewritten in this form. Please see the
API documentation on the  Lemur web site
 for more information about the generic scoring function form. 
*/

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


