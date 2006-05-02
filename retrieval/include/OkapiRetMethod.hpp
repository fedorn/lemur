/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _OKAPIRETMETHOD_HPP
#define _OKAPIRETMETHOD_HPP

#include "TextQueryRep.hpp"
#include "ScoreFunction.hpp"
#include "TextQueryRetMethod.hpp"
#include <cmath>

/// Okapi retrieval model parameter namespace

namespace OkapiParameter {
  struct TFParam {
    double k1;
    double b;
    double k3;
  }; 

  static double defaultK1 = 1.2;
  static double defaultB = 0.75;
  static double defaultK3 = 7;
  
  struct FeedbackParam {
    /// The (pseudo) count of a new term added to the query (It's unclear how this is set from the Okapi paper, so it's implemented as a parameter.)
    double expQTF; // expanded query term TF
    /// The number of terms to add for feedback
    int howManyTerms;
  };

  static double defaultExpQTF = 0.5;
  static int defaultHowManyTerms = 50;
}
namespace lemur 
{
  namespace retrieval 
  {
    
    /// Represent of a query term in Okapi retrieval model, the term carries a count of the number of rel docs with the term

    class OkapiQueryTerm : public lemur::api::QueryTerm {
    public:
      OkapiQueryTerm(lemur::api::TERMID_T termID, double count, int pEstCount, double paramK3) : QueryTerm(termID, count), pEst(pEstCount), k3(paramK3) {
      }
      /// return the number of rel docs with the term
      virtual int pEstCount() const { return pEst;}

      /// return query term TF weight
      virtual double weight() const { 
        return ((k3+1)*w/(k3+w));
      } 
    private:
      int pEst;
      double k3;
    };

    /// The Okapi scoring function

    class OkapiScoreFunc : public lemur::api::ScoreFunction {
    public:
      OkapiScoreFunc(const lemur::api::Index &dbIndex): ind(dbIndex) {}
      virtual double matchedTermWeight(const lemur::api::QueryTerm *qTerm, const lemur::api::TextQueryRep *qRep, const lemur::api::DocInfo *info, const lemur::api::DocumentRep *dRep) const;
    protected:
      const lemur::api::Index &ind;
    };


    /// OkapiQueryRep carries an array to store the count of relevant docs with a term 
    class OkapiQueryRep : public ArrayQueryRep {
    public:
      // initial query constructor, no feedback docs assumed
      OkapiQueryRep(const lemur::api::TermQuery &qry, const lemur::api::Index &dbIndex, double paramK3);

      virtual ~OkapiQueryRep() { delete [] pEst; }
      /// return total number of relevant/feedback documents
      int pNormCount() const { return pNorm;}
      /// set total number of relevant/feedback documents
      void setPNormCount(int count) { pNorm = count;}
      /// increase the count of relevant/feedback doc in which a term occurs
      void incPEst(int wdIndex, int val) { pEst[wdIndex]+=val;}
    protected:
      virtual lemur::api::QueryTerm *makeQueryTerm(lemur::api::TERMID_T wdIndex, double wdCount) const{
        return (new OkapiQueryTerm(wdIndex, wdCount, pEst[wdIndex], k3));
      }
      double k3;
      int pNorm;
      int *pEst;
    };

    /// Doc representation for Okapi model

    class OkapiDocRep : public lemur::api::DocumentRep {
    public:
      OkapiDocRep(lemur::api::DOCID_T docID, const lemur::api::Index &dbIndex, OkapiParameter::TFParam &param) : lemur::api::DocumentRep(docID, dbIndex.docLength(docID)), ind(dbIndex),
                                                                                                                 prm(param) {
      }
      virtual ~OkapiDocRep() { }
      virtual double termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo *info) const;
      double BM25TF(double rawTF, double docLength) const;
      virtual double scoreConstant() const { return 0;}
    protected:
      const lemur::api::Index &ind;
      OkapiParameter::TFParam &prm;
    };

    /// The Okapi BM25 retrieval function, as described in their TREC-3 paper

    class OkapiRetMethod : public lemur::api::TextQueryRetMethod  {
    public:


      OkapiRetMethod(const lemur::api::Index &dbIndex, lemur::api::ScoreAccumulator &accumulator);

      virtual ~OkapiRetMethod() { delete scFunc;}

      virtual lemur::api::TextQueryRep *computeTextQueryRep(const lemur::api::TermQuery &qry) {
        return (new OkapiQueryRep(qry, ind, tfParam.k3));
      }

      virtual lemur::api::DocumentRep *computeDocRep(lemur::api::DOCID_T docID) {
        return (new OkapiDocRep(docID, ind, tfParam));
      }

      virtual lemur::api::ScoreFunction *scoreFunc();

      /// It's suspected that there is a bug in the implementation of feedback; the performance is not as expected.   
      virtual void updateTextQuery(lemur::api::TextQueryRep &origRep, const lemur::api::DocIDSet &relDocs);

      void setTFParam(OkapiParameter::TFParam &tfWeightParam);

      void setFeedbackParam(OkapiParameter::FeedbackParam &feedbackParam);

      /// Compute the Robertson Spark Jones Weight 
      static double RSJWeight(double r, double R, 

                              double n, double N) {
        return (log ((r+0.5)*(N-n-R+r+0.5)/((R-r+0.5)*(n-r+0.5))));
      }

    protected:
      OkapiScoreFunc *scFunc;

      OkapiParameter::TFParam tfParam;
      OkapiParameter::FeedbackParam fbParam;


    };


    inline void OkapiRetMethod::setTFParam(OkapiParameter::TFParam &tfWeightParam)
    {
      tfParam = tfWeightParam;
    }

    inline void OkapiRetMethod::setFeedbackParam(OkapiParameter::FeedbackParam &feedbackParam)
    {
      fbParam = feedbackParam;
    }

    inline double OkapiDocRep::BM25TF(double rawTF, double docLength) const 
    {
      return ((prm.k1+1)*rawTF/(rawTF +  prm.k1*(1-prm.b + prm.b*docLength/ind.docLengthAvg())));
    }


    inline double OkapiDocRep::termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo *info) const
    {
      return BM25TF(info->termCount(), ind.docLength(info->docID()));
    } 

    inline lemur::api::ScoreFunction *OkapiRetMethod::scoreFunc()
    {
      return scFunc;
    }
  }
}


#endif /* _OKAPIRETMETHOD_HPP */
