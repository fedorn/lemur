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
/*
  Author: dmf 9/2002
*/

#ifndef _COSSIMRETMETHOD_HPP
#define _COSSIMRETMETHOD_HPP

#include "TextQueryRetMethod.hpp"

/// Parameters used in the CosSim retrieval method
namespace CosSimParameter {
  struct FeedbackParam {
    int howManyTerms;
    double posCoeff;
  };    
  static int defaultHowManyTerms = 50;
  static double defaultPosCoeff = 0.5;
  static const string defaultL2File = "cos.L2";
}
namespace lemur
{
  namespace retrieval 
  {
    
    /// Representation of a query (as a weighted vector) in the CosSim method
    class CosSimQueryRep : public ArrayQueryRep {
    public:
      /// Create a query representation for a text query.
      CosSimQueryRep(const lemur::api::TermQuery &qry, const lemur::api::Index &dbIndex, 
                     double *idfValue);
      /// Create a query representation for the given document id.
      CosSimQueryRep(lemur::api::DOCID_T docId, const lemur::api::Index &dbIndex, double *idfValue);
      virtual ~CosSimQueryRep() {}
    protected:
      double *idf;
      const lemur::api::Index &ind;
    };

    /// Representation of a doc (as a weighted vector) in the CosSim method.
    class CosSimDocRep : public lemur::api::DocumentRep {
    public:
      CosSimDocRep(lemur::api::DOCID_T docID, double *idfValue, double norm, int dl) : 
        lemur::api::DocumentRep(docID, dl), idf(idfValue), dNorm(norm) {
      }
      virtual ~CosSimDocRep() { }
      /// return dtf*idf.
      virtual double termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo *info) const { 
        return (idf[termID]*info->termCount()); 
      }
      /// return the L2 norm for this document.
      virtual double scoreConstant() const { return dNorm;}

    private:
      /// idf vector for collection.
      double *idf;
      /// L2 norm for this document.
      double dNorm; 
    };

    /// The ScoreFunction for the CosSim method.
    class CosSimScoreFunc : public lemur::api::ScoreFunction {
    public:
      CosSimScoreFunc(const lemur::api::Index &dbIndex): ind(dbIndex) {}
      /// score adjustment (divide by L2 Norm for document)
      virtual double adjustedScore(double origScore, const lemur::api::TextQueryRep *qRep, 
                                   const lemur::api::DocumentRep *dRep) const {
        return origScore/dRep->scoreConstant();
      }
    protected:
      const lemur::api::Index &ind;
    };

    /// Cosine similarity retrieval method.
    class CosSimRetMethod : public lemur::api::TextQueryRetMethod {
    public:
      /// Precomputes the idf values for the collection.
      CosSimRetMethod(const lemur::api::Index &dbIndex, lemur::api::ScoreAccumulator &accumulator);
      /// Precomputes the idf values for the collection. Loads the
      /// precomputed L2 document norms from L2File (if present).
      /// This support file is optional, if it is not found, L2 norms
      /// will be computed during scoring and cached across queries.
      CosSimRetMethod(const lemur::api::Index &dbIndex, const string &L2file,
                      lemur::api::ScoreAccumulator &accumulator);

      virtual ~CosSimRetMethod();

      virtual lemur::api::TextQueryRep *computeTextQueryRep(const lemur::api::TermQuery &qry) {
        return (new CosSimQueryRep(qry, ind, idfV));
      }

      virtual lemur::api::TextQueryRep *computeTextQueryRep(lemur::api::DOCID_T docid) {
        return (new CosSimQueryRep(docid, ind, idfV));
      }

      virtual lemur::api::DocumentRep *computeDocRep(lemur::api::DOCID_T docID) { 
        return (new CosSimDocRep(docID, idfV, docNorm(docID), 
                                 ind.docLength(docID)));
      }
      virtual lemur::api::ScoreFunction *scoreFunc() {
        return (scFunc);
      }
      virtual void updateTextQuery(lemur::api::TextQueryRep &qryRep, 
                                   const lemur::api::DocIDSet &relDocs);
      void setFeedbackParam(CosSimParameter::FeedbackParam &feedbackParam);
    protected:
  
      double *idfV;
      lemur::api::ScoreFunction *scFunc;
      double * docNorms;
      int numDocs, numTerms;

      /// @name Parameters
      //@{
      CosSimParameter::FeedbackParam fbParam;
      /// Name of support file containing the precomputed L2 norms.
      const string &L2FileName;
      //@}

      /// compute the L2 norm for a given docID.
      double docNorm(lemur::api::DOCID_T docID);
      /// load the precomputed L2 norms support file.
      void loadDocNorms();
    };

    inline void CosSimRetMethod::setFeedbackParam(CosSimParameter::FeedbackParam 
                                                  &feedbackParam) {
      fbParam = feedbackParam;
    }
  }
}

#endif /* _COSSIMRETMETHOD_HPP */
