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



