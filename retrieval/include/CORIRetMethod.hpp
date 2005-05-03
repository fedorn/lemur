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

#ifndef _CORIRETMETHOD_HPP
#define _CORIRETMETHOD_HPP


#include "UnigramLM.hpp"
#include "SimpleKLDocModel.hpp"
#include "TextQueryRetMethod.hpp"
#include "Param.hpp"
#include <math.h>
#include <iostream>

///The default tffactor tfbase values for the collection selection case and the document retrieval case
#define CSTFBASELINE 50
#define CSTFFACTOR 150
#define DOCTFBASELINE 0.5
#define DOCTFFACTOR 1.5
#define MINBELIEF 0.4
namespace lemur 
{
  /// document retrieval components.
  namespace retrieval
  {
    
    class CORIQueryRep : public ArrayQueryRep {
    public:
      CORIQueryRep(const lemur::api::TermQuery & qry, const lemur::api::Index & dbIndex);
      virtual ~CORIQueryRep() {}

    protected:
      const lemur::api::Index & ind;
    };

    class CORIDocRep : public lemur::api::DocumentRep {
    public:
      CORIDocRep(lemur::api::DOCID_T docID, const lemur::api::Index & dbIndex, double * cwRatio, 
                 double TFfact = 150, double TFbase = 50, 
                 const SimpleKLDocModel * smoother = NULL,
                 const lemur::langmod::UnigramLM * collectLM = NULL);
      virtual ~CORIDocRep() { }
      virtual double termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo * info) const ;

      virtual double scoreConstant() const { return 0; }

    private:

      const lemur::api::Index & ind;

      int * cwCounts;

      const SimpleKLDocModel * dfSmooth;
      const lemur::langmod::UnigramLM * collLM;

      double c05;
      double idiv;
      double tnorm;
    };



    class CORIRetMethod : public lemur::api::TextQueryRetMethod {
    public:

      CORIRetMethod(const lemur::api::Index & dbIndex, 
                    lemur::api::ScoreAccumulator &accumulator, 
                    lemur::utility::String cwName, int isCSIndex=0,
                    const SimpleKLDocModel ** smoothers = NULL, 
                    const lemur::langmod::UnigramLM * collectLM = NULL);
      ~CORIRetMethod() { delete scFunc; delete [] cwRatio; }

      virtual lemur::api::TextQueryRep * computeTextQueryRep(const lemur::api::TermQuery & qry) {
        return new CORIQueryRep(qry, ind);
      }
      virtual lemur::api::DocumentRep * computeDocRep(lemur::api::DOCID_T docID) { 
        if (dfSmooth != NULL) {
          return new CORIDocRep(docID, ind, cwRatio, tffactor, tfbaseline, dfSmooth[docID], collLM);
        }
        return new CORIDocRep(docID, ind, cwRatio, tffactor, tfbaseline);
      }
      virtual lemur::api::ScoreFunction * scoreFunc() {
        return scFunc;
      }

      virtual void scoreCollection(const lemur::api::QueryRep &qry, lemur::api::IndexedRealVector &results);

      virtual void updateTextQuery(lemur::api::TextQueryRep &qryRep, const lemur::api::DocIDSet &relDocs) { }
  
      void setTFFactor(double tf) { tffactor = tf; }
      void setTFBaseline(double tf) { tfbaseline = tf; }

    protected:

      lemur::api::ScoreFunction * scFunc;
      const SimpleKLDocModel ** dfSmooth;
      const lemur::langmod::UnigramLM * collLM;

      double * cwRatio;
      double tffactor;
      double tfbaseline;
  
    };

    class CORIScoreFunc : public lemur::api::ScoreFunction {
    public:
      CORIScoreFunc(const lemur::api::Index & index) : ind(index) {
        rmax=0;
        double dc = ind.docCount();
        c05 = dc + 0.5;
        idiv = log(dc + 1);
        //    qr=NULL;
        first=0;
      }

      virtual double adjustedScore(double origScore, const lemur::api::TextQueryRep * qRep,
                                   const lemur::api::DocumentRep * dRep) const {
        /*
          if (qr != qRep) {
          qr = qRep;
      
          qRep->startIteration();
          rmax = 0;
          double qw = 0;
          while (qRep->hasMore()) {
          lemur::api::TERMID_T qtid = qRep->nextTerm()->id();
          rmax += (1-MINBELIEF)*(log(c05 / ind.docCount(qtid)) / idiv);
          }
          }
          if ((origScore/rmax)>=1){
          cout<<"!!!!!!!!!"<<endl;
          cout<<origScore<<" "<<rmax<<" "<<(origScore / rmax)<<endl;
          }*/
        //return (origScore / rmax);
        return origScore;
      }

    private:
      const lemur::api::Index & ind;
      int first;
      //TextQueryRep * qr;
      double rmax;
      double c05;
      double idiv;
    };
  }
}


#endif /* _CORIRETMETHOD_HPP */
