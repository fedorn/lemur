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


#ifndef _DOCUMENTREP_HPP
#define _DOCUMENTREP_HPP

#include "DocInfoList.hpp"

//------------------------------------------------------------
//      Abstract Interface for Document Representation 
//------------------------------------------------------------

/// Representation of documents in a collection 

class DocumentRep {
public:
  DocumentRep(int docID) : id(docID) {}
  virtual ~DocumentRep() {}

  /// term weighting function
  virtual double termWeight(int termID, DocInfo *info) = 0;

  /// doc-specific constant term in the scoring formula
  virtual double scoreConstant() = 0 ;
protected:
  int id;
};



#endif /* _DOCUMENTREP_HPP */



