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

#include <math.h>
#include "UnigramLM.hpp"
#include "ScoreFunction.hpp"
#include "SimpleKLDocModel.hpp"
#include "TextQueryRep.hpp"
#include "TextQueryRetMethod.hpp"
#include "Counter.hpp"
#include "DocUnigramCounter.hpp"

namespace SimpleKLParameter {
  enum SmoothMethod  {JELINEKMERCER=0, DIRICHLETPRIOR=1, ABSOLUTEDISCOUNT=2};
  
  enum SmoothStrategy  {INTERPOLATE=0, BACKOFF=1}; 

  enum QueryUpdateMethod {MIXTURE = 0, DIVMIN=1, MARKOVCHAIN=2};

  struct DocSmoothParam {
    /// smoothing method
    enum SmoothMethod smthMethod;
    /// smoothing strategy
    enum SmoothStrategy smthStrategy;
    /// discount constant (delta) in absolute discounting
    double ADDelta;
    /// collection model coefficient (lambda) in Jelinek-Mercer
    double JMLambda;
    /// prior (mu) in Dirichlet prior
    double DirPrior;
  };

  static enum SmoothMethod defaultSmoothMethod = DIRICHLETPRIOR;
  static enum SmoothStrategy defaultSmoothStrategy = INTERPOLATE;
  static double defaultADDelta = 0.7;
  static double defaultJMLambda = 0.5;
  static double defaultDirPrior = 1000;

  struct QueryModelParam {
    /// query model re-estimation/updating method
    enum QueryUpdateMethod fbMethod;
    /// Q_new = (1-fbCoeff)*Q_old + fbCoeff*FBModel
    double fbCoeff;
    /// how many terms to use for the re-estimated query model
    int fbTermCount;
    /// feedback query model term probability threshold (only terms with a higher prob. will be used
    double fbPrTh;
    /// feedback query model prob. sum threshold (taking terms up to the point, where the accumulated prob. mass exceeds the threshold
    double fbPrSumTh;
    /// collection influence coefficient (e.g., in mixture model and divergence minimization methods)
    double fbMixtureNoise;
    //// max iterations for EM algorithm (will stop earlier if the likelihood converges with an error smaller than 0.5)
    int emIterations;
  };

  static enum QueryUpdateMethod defaultFBMethod = MIXTURE;
  static double defaultFBCoeff = 0.5;
  static int defaultFBTermCount =50;
  static double defaultFBPrTh = 0.001;
  static double defaultFBPrSumTh = 1;
  static double defaultFBMixNoise = 0.5;
  static int defaultEMIterations = 50;

};

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
  void computeMarkovChainFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs) 
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








