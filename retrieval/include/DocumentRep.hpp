#ifndef _DOCUMENTREP_HPP
#define _DOCUMENTREP_HPP

#include "DocInfoList.hpp"

//------------------------------------------------------------
//      Abstract Interface for Document Representation 
//------------------------------------------------------------

/// Representation of documents in a collection with default implementation

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



