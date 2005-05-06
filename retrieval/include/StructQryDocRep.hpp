/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _STRUCTQRYDOCREP_HPP
#define _STRUCTQRYDOCREP_HPP
#include "DocumentRep.hpp"
#include <cmath>

namespace lemur 
{
  namespace retrieval
  {
    
    /// \brief Representation of a document for structured query retrieval method.
    /// Provides support for passage level operations on a document.
    class StructQryDocRep : public lemur::api::DocumentRep {
    public:
      /// New StructQryDocRep
      StructQryDocRep(lemur::api::DOCID_T docID, double *idfValue, int docLength, int docCount,
                      double docLengthAverage, double db) : 
        lemur::api::DocumentRep(docID), did(docID), idf(idfValue), end(docLength),
        docEnd(docLength), size(docLength), start(0),
        dla(docLengthAverage), defaultBelief(db) {
        oneMinusDB = 1 - defaultBelief;
        denom = log(docCount + 1.0);
        numer = docCount + 0.5;
      }
      /// no clean up
      virtual ~StructQryDocRep() {}
      /// needed for DocRep interface.
      virtual double termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo *info) const { return 0;}
      /// Belief score for this term with this dtf.
      virtual double termWeight(lemur::api::TERMID_T termID, double dtf, int df) const{
        if (idf)
          return beliefScore(dtf, idf[termID]);
        else
          return beliefScore(dtf, computeIdfScore(df));
      }
      /// needed for DocRep interface.
      virtual double scoreConstant() const { return 0;}

      ///pass in passage size.
      void startPassageIteration(int sz) const {
        size = sz;
        increment = size/2;
        start = 0;
        end = size < docEnd ? size : docEnd;
      }
      /// any passages left?
      bool hasMorePassage() const {
        // still some terms in the list.
        return(start < docEnd);
      }

      /// next block of psgSize termids, empty positions == OOV (0);
      void nextPassage() const{
        if(start + increment < docEnd)
          start += increment;
        else
          start = docEnd;
        end = (start + size) < docEnd ? (start + size) : docEnd;
      }

      /// compute idf for the given document frequency as
      /// log((|C|+0.5)/df)/(log(|C|+1)
      double computeIdfScore(double df) const {
        return log(numer/df)/denom;
      }

      /// compute the belief score for a given tf/idf.
      double beliefScore(double df, double idf) const {
        return (defaultBelief + oneMinusDB
                * (df / (df + 0.5 + 1.5* ((end - start)/dla))) * idf);
      }

      /// the document id.
      lemur::api::DOCID_T did;
      /// start position of a passage
      mutable int start; 
      /// end position of a passage
      mutable int end;

    private:
      /// cached term idf values. May be NULL.
      double *idf;
      /// size used by passages
      mutable int size; 
      /// passage overlap value. default is size/2.
      mutable int increment; 
      /// length of the document.
      int docEnd;  
      /// average document length
      double dla;
      /// terms in idf formula
      double numer, denom;
      /// default belief and 1 - dB.
      double defaultBelief, oneMinusDB;
    };
  }
}

#endif
