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

#ifndef _TEXTQUERYRETMETHOD_HPP
#define _TEXTQUERYRETMETHOD_HPP


#include "RetrievalMethod.hpp"
#include "TextQueryRep.hpp"
#include "DocumentRep.hpp"
#include "ScoreFunction.hpp"
#include "ScoreAccumulator.hpp"
#include "FreqVector.hpp"

//----------------------------------------------------------------------
//      Abstract Interface for A Retrieval Method/Model for Text Query
//----------------------------------------------------------------------

/*!
  A text query retrieval method is determined by specifying the following elements <BR>
  <UL>
  <LI> A method to compute the query representation
  <LI> A method to compute the doc representation
  <LI> The scoring function
  <LI> A method to update the query representation based on a set of (relevant) documents
  </UL>
*/

class TextQueryRetMethod : public RetrievalMethod {
public:
  TextQueryRetMethod(Index &ind, ScoreAccumulator & accumulator) : 
    RetrievalMethod(ind), scAcc(accumulator) {}
  virtual ~TextQueryRetMethod() {}
  /// compute the query representation for a text query (caller responsible for deleting the memory of the generated new instance)
  virtual TextQueryRep *computeTextQueryRep(TextQuery &qry)=0;

  /// overriding abstract class methods
  virtual QueryRep *computeQueryRep(Query &qry); 
  virtual double scoreDoc(QueryRep &qry, int docID);
  virtual void scoreCollection(QueryRep &qry, IndexedRealVector &results);

  /// compute the doc representation (caller responsible for deleting the memory of the generated new instance)
  virtual DocumentRep *computeDocRep(int docID) =0;
  /// return the scoring function pointer
  virtual ScoreFunction *scoreFunc() = 0;
  /// update the query
  virtual void updateQuery(QueryRep &qryRep, DocIDSet &relDocs) {
    updateTextQuery(*((TextQueryRep *)(&qryRep)), relDocs);
  }
  virtual void updateTextQuery(TextQueryRep &qryRep, DocIDSet &relDocs)=0;

  /// Efficient scoring with the inverted index
  /*! a general scoring procedure shared by many different models (assuming "sortedScores has memory allocated)
 
  */
  virtual void scoreInvertedIndex(QueryRep &qryRep, IndexedRealVector &scores, bool scoreAll=false);

  virtual double scoreDocVector(TextQueryRep &qry, int docID, FreqVector &docVector);

protected:
  ScoreAccumulator &scAcc;
};


//=============== inlines ========================================

inline QueryRep *TextQueryRetMethod::computeQueryRep(Query &qry) { 
  TextQuery *q = static_cast<TextQuery *>(&qry);
  return (computeTextQueryRep(*q));
}

#endif /* _TEXTQUERYRETMETHOD_HPP */




