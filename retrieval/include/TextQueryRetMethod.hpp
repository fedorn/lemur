/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
*/


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


Given a query <tt>q =(q<sub>1</sub>,q<sub>2</sub>,...,q<sub>N</sub>)</tt> 
and a document <tt>d=(d<sub>1</sub>,d<sub>2</sub>,...,d<sub>N</sub>)</tt>, 
where <tt>q<sub>1</sub>,...,q<sub>N</sub></tt> and <tt>d<sub>1</sub>,...,d<sub>N</sub></tt> are terms, <tt>TextQueryRetMethod</tt> assumes the following general scoring function:

<blockquote>
<pre>
s(q,d) = g(w(q<sub>1</sub>,d<sub>1</sub>,q,d) + ... + w(q<sub>N</sub>,d<sub>N</sub>,q,d),q,d)
</pre>
</blockquote>
That is, the score of a document <tt>d</tt> against a query <tt>q</tt> is a 
function <tt>g</tt> of the accumulated weight <tt>w</tt> for each matched term.
 
The score is thus determined by
two functions <tt>g</tt> and <tt>w</tt>; both may depend on the whole query or 
document. The function <tt>w</tt> gives the weight of each matched term, 
while the function <tt>g</tt> makes it possible to perform any 
further transformation of the sum of the weight of all matched terms based on the 
"summary" information of a query or a document (e.g., document length). 
<p>
<tt>TextQueryRep</tt>, <tt>DocumentRep</tt>, and <tt>ScoreFunction</tt> are 
designed to support this general
scoring function in the following way: 
<p>
A <tt>ScoreFunction</tt> is responsible for defining the two functions
<tt>g</tt> and <tt>w</tt>.  A <tt>TextQueryRep</tt> provides any information
required for scoring from the query side (e.g., query term
frequency). Similarly, a <tt>DocumentRep</tt> provides any information required
for scoring from the document side. Furthermore, a <tt>TextQueryRep</tt> supports
iteration over all query terms, allowing easy accumulation of weights
over matched terms. The weighting function <tt>w</tt> and score adjustment
function <tt>g</tt> typically assume and depend on some particular information
and representation of the query and document, so a specific
<tt>ScoreFunction</tt> (for a specific retrieval method) only works for some
specific <tt>TextQueryRep</tt> and <tt>DocumentRep</tt> that are appropriate for the specific retrieval method.

*/

class TextQueryRetMethod : public RetrievalMethod {
public:
  /// Create the retrieval method. If cacheDocReps is true,
  /// allocate DocumentRep cache array.
  TextQueryRetMethod(Index &ind, ScoreAccumulator & accumulator);
  /// Destroy the object. If cacheDocReps is true, delete the
  /// DocumentRep cache array
  virtual ~TextQueryRetMethod() {
    if (cacheDocReps) {
      for (int i = 0; i < docRepsSize; i++) delete(docReps[i]);
      delete[](docReps);
    }
  }

  /// compute the query representation for a text query (caller responsible for deleting the memory of the generated new instance)
  virtual TextQueryRep *computeTextQueryRep(TextQuery &qry)=0;
  /// compute a query rep for an existing doc (DOCID_T needed).
  virtual TextQueryRep *computeTextQueryRep(int docid){
    return NULL; 
  } // need to implement for other ret methods.
  /// overriding abstract class method
  virtual QueryRep *computeQueryRep(Query &qry); 

  /// ooverriding abstract class method
  virtual double scoreDoc(QueryRep &qry, int docID);
  /// ooverriding abstract class method with a general efficient inverted index scoring procedure
  virtual void scoreCollection(QueryRep &qry, IndexedRealVector &results);
  /// add support for scoring an existing document against the collection
  virtual void scoreCollection(int docid, IndexedRealVector &results);
  
  /// compute the doc representation (caller responsible for deleting the memory of the generated new instance)
  virtual DocumentRep *computeDocRep(int docID) =0;
  /// return the scoring function pointer
  virtual ScoreFunction *scoreFunc() = 0;
  /// update the query
  virtual void updateQuery(QueryRep &qryRep, DocIDSet &relDocs) {
    updateTextQuery(*((TextQueryRep *)(&qryRep)), relDocs);
  }

  /// Modify/update the query representation based on a set (presumably) relevant documents
  virtual void updateTextQuery(TextQueryRep &qryRep, DocIDSet &relDocs)=0;

  /// Efficient scoring with the inverted index
  /*! a general scoring procedure shared by many different models (assuming "sortedScores has memory allocated)
 
  */
  virtual void scoreInvertedIndex(QueryRep &qryRep, IndexedRealVector &scores, bool scoreAll=false);

  virtual double scoreDocVector(TextQueryRep &qry, int docID, FreqVector &docVector);

protected:
  ScoreAccumulator &scAcc;
  /// cache document reps.
  DocumentRep **docReps;
  /// whether or not to cache document representations
  bool cacheDocReps;
  /// number of documents plus 1, the size of the docReps array.
  int docRepsSize;
};


//=============== inlines ========================================

inline QueryRep *TextQueryRetMethod::computeQueryRep(Query &qry) { 
  TextQuery *q = static_cast<TextQuery *>(&qry);
  return (computeTextQueryRep(*q));
}

#endif /* _TEXTQUERYRETMETHOD_HPP */




