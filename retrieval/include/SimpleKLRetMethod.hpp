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


#ifndef _SIMPLEKLRETMETHOD_HPP
#define _SIMPLEKLRETMETHOD_HPP

#include <cmath>
#include "UnigramLM.hpp"
#include "ScoreFunction.hpp"
#include "SimpleKLDocModel.hpp"
#include "TextQueryRep.hpp"
#include "TextQueryRetMethod.hpp"
#include "Counter.hpp"
#include "DocUnigramCounter.hpp"

/// Query model representation for the simple KL divergence model

class SimpleKLQueryModel : public ArrayQueryRep {
public:
  /// construct a query model based on query text
  SimpleKLQueryModel(TextQuery &qry, Index &dbIndex) : ArrayQueryRep(dbIndex.termCountUnique()+1, qry, dbIndex), qm(NULL), ind(dbIndex) {
  }

  /// construct an empty query model
  SimpleKLQueryModel(Index &dbIndex) : ArrayQueryRep(dbIndex.termCountUnique()+1), qm(NULL), ind(dbIndex) {
    startIteration();
    while (hasMore()) {
      QueryTerm *qt = nextTerm();
      setCount(qt->id(), 0);
    }

  }


  virtual ~SimpleKLQueryModel(){ if (qm) delete qm;}


  /// interpolate the model with any (truncated) unigram LM, default parameter  to control the truncation is the number of words
  virtual void interpolateWith(UnigramLM &qModel, double origModCoeff, int howManyWord, double prSumThresh=1, double prThresh=0);
  virtual double scoreConstant() {
    return totalCount();
  }
  
  /// load a query model/rep from input stream is
  virtual void load(istream &is);

  /// save a query model/rep to output stream os
  virtual void save(ostream &os);

private:
  IndexedRealVector *qm;
  Index &ind;
};



/// Simple KL-divergence scoring function

class SimpleKLScoreFunc : public ScoreFunction {
public:

  virtual double matchedTermWeight(QueryTerm *qTerm, TextQueryRep *qRep, DocInfo *info, DocumentRep *dRep) { 
    return (qTerm->weight()*log(dRep->termWeight(qTerm->id(),info)));
  }

  /// score adjustment (e.g., appropriate length normalization)
  virtual double adjustedScore(double origScore, TextQueryRep *qRep, DocumentRep *dRep) {
    SimpleKLQueryModel *qm = (SimpleKLQueryModel *)qRep;
    // dynamic_cast<SimpleKLQueryModel *>qRep;
    SimpleKLDocModel *dm = (SimpleKLDocModel *)dRep;
      // dynamic_cast<SimpleKLDocModel *>dRep;
    return (origScore+log(dm->scoreConstant())*qm->scoreConstant());
  }
};


/// KL Divergence retrieval model with simple document model smoothing


class SimpleKLRetMethod : public TextQueryRetMethod {
public:

  SimpleKLRetMethod(Index &dbIndex, const char *supportFileName, ScoreAccumulator &accumulator);
  virtual ~SimpleKLRetMethod();
  
  virtual TextQueryRep *computeTextQueryRep(TextQuery &qry) {
    return (new SimpleKLQueryModel(qry, ind));
  }
  
  virtual DocumentRep *computeDocRep(int docID);
  

  virtual ScoreFunction *scoreFunc() {
    return (scFunc);
  }
  

  virtual void updateTextQuery(TextQueryRep &origRep, DocIDSet &relDocs);

  void setDocSmoothParam(SimpleKLParameter::DocSmoothParam &docSmthParam);
  void setQueryModelParam(SimpleKLParameter::QueryModelParam &queryModParam);

protected:

  double *mcNorm; // needed for fast one-step Markov chain   
  
  double *docProbMass; // needed for fast alpha computing
  int *uniqueTermCount; // needed for supporting fast absolute discounting
  UnigramLM *collectLM; // a little faster if pre-computed
  DocUnigramCounter *collectLMCounter; // support collectLM
  SimpleKLScoreFunc *scFunc; // keep a copy to be used at any time

  /// @name query model updating methods (i.e., feedback methods)
  //@{
  /// Mixture model feedback method
  void computeMixtureFBModel(SimpleKLQueryModel &origRep, DocIDSet & relDocs);
  /// Divergence minimization feedback method
  void computeDivMinFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs);
  /// Markov chain feedback method
  void computeMarkovChainFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs) ;
  //@}

  SimpleKLParameter::DocSmoothParam docParam;
  SimpleKLParameter::QueryModelParam qryParam;

};


inline  void SimpleKLRetMethod::setDocSmoothParam(SimpleKLParameter::DocSmoothParam &docSmthParam)
{
  docParam = docSmthParam;
}

inline  void SimpleKLRetMethod::setQueryModelParam(SimpleKLParameter::QueryModelParam &queryModParam)
{
  qryParam = queryModParam;
}

#endif /* _SIMPLEKLRETMETHOD_HPP */








