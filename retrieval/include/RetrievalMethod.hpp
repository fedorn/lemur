#ifndef _RETRIEVALMETHOD_HPP
#define _RETRIEVALMETHOD_HPP

#include "QueryRep.hpp"
#include "DocumentRep.hpp"
#include "ScoreFunction.hpp"

//------------------------------------------------------------
//      Abstract Interface for A Retrieval Method/Model
//------------------------------------------------------------

/*!
  A retrieval method is determined by specifying the following elements <BR>
  <UL>
  <LI> A method to compute the query representation
  <LI> A method to compute the doc representation
  <LI> The scoring function
  <LI> A method to update the query representation based on a set of (relevant) documents
  </UL>
*/

class RetrievalMethod {
public:
  /// compute the query representation for a text query (caller responsible for deleting the memory of the generated new instance)
  virtual QueryRep *computeQueryRep(TextQuery &qry)=0;
  /// compute the doc representation (caller responsible for deleting the memory of the generated new instance)
  virtual DocumentRep *computeDocRep(int docID) =0;
  /// return the scoring function pointer
  virtual ScoreFunction *scoreFunc() = 0;
  /// update the query
  virtual void updateQuery(QueryRep &qryRep, DocIDSet &relDocs) = 0;
  /// return the Index handle
  virtual Index *index() { return ind;}
protected:
  Index *ind;
};

#endif /* _RETRIEVALMETHOD_HPP */









