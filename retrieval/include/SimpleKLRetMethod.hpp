#ifndef _SIMPLEKLRETMETHOD_HPP
#define _SIMPLEKLRETMETHOD_HPP

#include <math.h>
#include "UnigramLM.hpp"
#include "ScoreFunction.hpp"
#include "SimpleKLDocModel.hpp"
#include "QueryRep.hpp"
#include "RetrievalMethod.hpp"
#include "Counter.hpp"
#include "DocUnigramCounter.hpp"

/// Query model representation for the simple KL divergence model

class SimpleKLQueryModel : public ArrayQueryRep {
public:
  /// construct a query model based on query text
  SimpleKLQueryModel(TextQuery &qry, Index &dbIndex) : ArrayQueryRep(dbIndex.termCountUnique()+1, qry, dbIndex), qm(NULL), ind(dbIndex) {
  }

  /// construct an empty query model
  SimpleKLQueryModel(int size, Index &dbIndex) : ArrayQueryRep(size), qm(NULL), ind(dbIndex) {
    startIteration();
    while (hasMore()) {
      QueryTerm *qt = nextTerm();
      setCount(qt->id(), 0);
    }

  }


  ~SimpleKLQueryModel(){ if (qm) delete qm;}


  /// interpolate the model with any (truncated) unigram LM, default parameter  to control the truncation is the number of words
  void interpolateWith(UnigramLM &qModel, double origModCoeff, int howManyWord, double prSumThresh=1, double prThresh=0);
  virtual double scoreConstant() {
    return totalCount();
  }
  
private:
  IndexedRealVector *qm;
  Index &ind;
};



/// Simple KL-divergence scoring function

class SimpleKLScoreFunc : public ScoreFunction {
public:

  virtual double matchedTermWeight(QueryTerm *qTerm, QueryRep *qRep, DocInfo *info, DocumentRep *dRep) { 
    return (qTerm->weight()*log(dRep->termWeight(qTerm->id(),info)));
  }

  /// score adjustment (e.g., appropriate length normalization)
  virtual double adjustedScore(double origScore, QueryRep *qRep, DocumentRep *dRep) {
    SimpleKLQueryModel *qm = (SimpleKLQueryModel *)qRep;
    // dynamic_cast<SimpleKLQueryModel *>qRep;
    SimpleKLDocModel *dm = (SimpleKLDocModel *)dRep;
      // dynamic_cast<SimpleKLDocModel *>dRep;
    return (origScore+log(dm->scoreConstant())*qm->scoreConstant());
  }
};


/// KL Divergence retrieval model with simple document model smoothing


class SimpleKLRetMethod : public RetrievalMethod {
public:
  enum SmoothMethod  {JELINEKMERCER=0, BAYESIAN=1, ABSOLUTEDISCOUNT=2};
  
  enum SmoothStrategy  {INTERPOLATE=0, BACKOFF=1}; 

  enum QueryUpdateMethod {MAXLIKE = 0, MIXTURE = 1, DIVMIN=2, MARKOVCHAIN=3};

  struct Parameter {
    int smthMethod;
    int smthStrategy;
    int fbMethod;
    /// Q_new = (1-fbCoeff)*Q_old + fbCoeff*FBModel
    double fbCoeff;  
    int fbTermCount;
    double fbPrTh;
    double fbPrSumTh;
    double fbMixtureNoise;
    /// p(w) = (1-fbMixtureCoeff)*p(w|feedbackModel)+fbMixtureCoeff*p(w|collect)
    double fbMixtureCoeff; 
    int emIterations;
    double ADDelta;
    double JMLambda;
    double DirPrior;
    double qFidelity;
  };
  Parameter param;

  SimpleKLRetMethod(Index &dbIndex);
  
  virtual ~SimpleKLRetMethod();
  
  virtual QueryRep *computeQueryRep(TextQuery &qry) {
    return (new SimpleKLQueryModel(qry, *ind));
  }
  
  virtual DocumentRep *computeDocRep(int docID);
  

  virtual ScoreFunction *scoreFunc() {
    return (scFunc);
  }
  

  virtual void updateQuery(QueryRep &origRep, DocIDSet &relDocs);


protected:
  
  double *docProbMass; // needed for fast alpha computing
  int *uniqueTermCount; // needed for supporting fast absolute discounting
  UnigramLM *collectLM; // a little faster if pre-computed
  DocUnigramCounter *collectLMCounter; // support collectLM
  SimpleKLScoreFunc *scFunc; // keep a copy to be used at any time
private:
  /// Markov chain feedback method
  MLUnigramLM *computeMCQueryModel(ArrayCounter<double> &counter,
				   SimpleKLQueryModel &origRep,
				   DocIDSet &relDocs,
				   Index &ind, 
				   int stopWordCutoff, 
				   double alpha);
  /// Mixture model feedback method
  void computeMixtureFBModel(SimpleKLQueryModel &origRep, DocIDSet & relDocs);
  /// Divergence minimization feedback method
  void computeDivMinFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs);
};

#endif /* _SIMPLEKLRETMETHOD_HPP */








