/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
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
  SimpleKLQueryModel(TextQuery &qry, Index &dbIndex) : ArrayQueryRep(dbIndex.termCountUnique()+1, qry, dbIndex), qm(NULL), ind(dbIndex), colKLComputed(false) {
  }

  /// construct an empty query model
  SimpleKLQueryModel(Index &dbIndex) : ArrayQueryRep(dbIndex.termCountUnique()+1), qm(NULL), ind(dbIndex), colKLComputed(false) {
    startIteration();
    while (hasMore()) {
      QueryTerm *qt = nextTerm();
      setCount(qt->id(), 0);
      delete qt;
    }

  }


  virtual ~SimpleKLQueryModel(){ if (qm) delete qm;}


  /// interpolate the model with any (truncated) unigram LM, default parameter  to control the truncation is the number of words
  /*!
    The interpolated model is defined as <tt> origModCoeff</tt>*p(w|original_model)+(1-<tt>origModCoeff</tt>*p(w|new_truncated_model).
<p> The "new truncated model" gives a positive probability to all words that "survive" in the truncating process, but gives a zero probability to all others.
So, the sum of all word probabilities according to the truncated model does not
have to sum to 1. The assumption is that if a word has an extrememly small probability, adding it to the query model will not affect scoring that much. <p> The truncation procedure is as follows:  First, we sort the probabilities in <tt> qModel</tt> passed in, and then iterate over all the entries. For each entry, we check the stopping condition and add the entry to the existing query model if none of the following stopping conditions is satisfied. If, however, any of the conditions is satisfied, the process will terminate. The three stopping conditions are: (1) We already added <tt>howManyWord</tt> words. (2) The total sum of probabilities added exceeds the threshold <tt>prSumThresh</tt>. (3) The probability of the current word is below <tt>prThresh</tt>.
   */

  virtual void interpolateWith(UnigramLM &qModel, double origModCoeff, int howManyWord, double prSumThresh=1, double prThresh=0);
  virtual double scoreConstant() {
    return totalCount();
  }
  
  /// load a query model/rep from input stream is
  virtual void load(istream &is);

  /// save a query model/rep to output stream os
  virtual void save(ostream &os);

  /// save a query clarity to output stream os
  virtual void clarity(ostream &os);
  /// compute query clarity score
  virtual double clarity();


  /// get and compute if necessary query-collection KL-div (useful for recovering the true divergence value from a score)
  double colDivergence() {
    if (colKLComputed) {
      return colKL;
    } else {
      colKLComputed = true;
      double d=0;
      startIteration();
      while (hasMore()) {
	QueryTerm *qt=nextTerm();
	double pr = qt->weight()/(double)totalCount();
	double colPr = (ind.termCount(qt->id())+1)/(double)(ind.termCount()+ind.termCountUnique()); // Laplace smoothing, same as in SimpleKLRetMethod
	d += pr*log(pr/colPr);
	delete qt;
	
      }
      colKL=d;
      return d;
    }
  }



  /// compute the KL-div of the query model and any unigram LM, i.e.,D(Mq|Mref)
  double KLDivergence(UnigramLM &refMod) {
    double d=0;
    startIteration();
    while (hasMore()) {
      QueryTerm *qt=nextTerm();
      double pr = qt->weight()/(double)totalCount();
      d += pr*log(pr/refMod.prob(qt->id()));
      delete qt;
    }
    return d;
  }



protected:

  double colKL;
  bool colKLComputed;

  IndexedRealVector *qm;
  Index &ind;
};



/// Simple KL-divergence scoring function
/*! 
 The KL-divergence formula D(model_q || model_d), when used for ranking
documents, can be computed
efficiently by re-writing the formula as a sum over all matched
terms in a query and a document. The details of such rewriting are
described in the following two papers:
<ul>
<li>C. Zhai and J. Lafferty. A study of smoothing methods for language models applied to ad hoc
      information retrieval, In 24th ACM SIGIR Conference on Research and Development in Information
      Retrieval (SIGIR'01), 2001. 
<li>P. Ogilvie and J. Callan. Experiments using the Lemur toolkit. In Proceedings of the Tenth Text
      Retrieval Conference (TREC-10). 
</ul>
*/

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

    /// The following are three different options for scoring

    /// ==== Option 1: query likelihood ==============
    // this is the original query likelihood scoring formula
    //  return (origScore+log(dm->scoreConstant())*qm->scoreConstant());

    /// ==== Option 2: cross-entropy (normalized query likelihood) ==== 
    // This is the normalized query-likelihood, i.e., cross-entropy
    // assert(qm->scoreConstant()!=0);
    // return (origScore/qm->scoreConstant() + log(dm->scoreConstant()));

    /// ==== Option 3: negative KL-divergence ==== 
    // This is the exact (negative) KL-divergence value, i.e., -D(Mq||Md)
    assert(qm->scoreConstant()!=0);
    return (origScore/qm->scoreConstant() + log(dm->scoreConstant())
	    - qm->colDivergence());


  }
};




/// KL Divergence retrieval model with simple document model smoothing


class SimpleKLRetMethod : public TextQueryRetMethod {
public:

  /// Construction of SimpleKLRetMethod requires a smoothing support file, which can be generated by the application GenerateSmoothSupport. The use of this smoothing support file is to store some pre-computed quantities so that the scoring procedure can be speeded up.
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

/// needed for fast one-step Markov chain   
  double *mcNorm; 
  
/// needed for fast alpha computing
  double *docProbMass; 
/// needed for supporting fast absolute discounting
  int *uniqueTermCount; 
/// a little faster if pre-computed
  UnigramLM *collectLM; 
  /// support the construction of collectLM
  DocUnigramCounter *collectLMCounter; 
  /// keep a copy to be used at any time
  SimpleKLScoreFunc *scFunc; 

  /// @name query model updating methods (i.e., feedback methods)
  //@{
  /// Mixture model feedback method
  void computeMixtureFBModel(SimpleKLQueryModel &origRep, DocIDSet & relDocs);
  /// Divergence minimization feedback method
  void computeDivMinFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs);
  /// Markov chain feedback method
  void computeMarkovChainFBModel(SimpleKLQueryModel &origRep, DocIDSet &relDocs) ;
  /// Relevance model1 feedback method
  void computeRM1FBModel(SimpleKLQueryModel &origRep, DocIDSet & relDocs);
  /// Relevance model1 feedback method
  void computeRM2FBModel(SimpleKLQueryModel &origRep, DocIDSet & relDocs);
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








