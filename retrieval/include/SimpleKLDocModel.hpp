/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */

#ifndef _SIMPLEKLDOCMODEL_HPP
#define _SIMPLEKLDOCMODEL_HPP

#include "DocumentRep.hpp"
#include "Index.hpp"
#include "UnigramLM.hpp"


/// Simple KL divergence retrieval model parameters
namespace SimpleKLParameter {
  enum SmoothMethod  {JELINEKMERCER=0, DIRICHLETPRIOR=1, ABSOLUTEDISCOUNT=2, 
                      TWOSTAGE=3};
 
  enum SmoothStrategy  {INTERPOLATE=0, BACKOFF=1};

  enum QueryUpdateMethod {MIXTURE = 0, DIVMIN=1, MARKOVCHAIN=2, RM1=3, RM2=4};

  enum adjustedScoreMethods {QUERYLIKELIHOOD = 1, CROSSENTROPY = 2, 
                             NEGATIVEKLD = 3};

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
    enum adjustedScoreMethods adjScoreMethod;
    /// query noise coefficient
    double qryNoise;

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
  static double defaultQryNoise = 0; //maximum likelihood estimator
}

namespace lemur 
{
  namespace retrieval
  {
    

    /// Doc representation for simple KL divergence retrieval model

    /*!
      abstract interface of doc representation for smoothed document unigram model
 
      adapt a smoothed document language model interface to a DocumentRep interface
      <PRE>
      p(w|d) = q(w|d) if w seen
      = a(d) * Pc(w)  if w unseen
      where,  a(d) controls the probability mass allocated to all unseen words and     Pc(w) is the collection language model
      </PRE>

    */

    class SimpleKLDocModel : public lemur::api::DocumentRep {
    public:
      SimpleKLDocModel(lemur::api::DOCID_T docID, const lemur::langmod::UnigramLM &collectLM, 
                       int dl = 1, 
                       const double *prMass = NULL,
                       SimpleKLParameter::SmoothStrategy strat = SimpleKLParameter::INTERPOLATE) : 
        lemur::api::DocumentRep(docID, dl), 
        refLM(collectLM), docPrMass(prMass), strategy(strat) {
      };
  
      ~SimpleKLDocModel() {};

      /// term weighting function, weight(w) = p_seen(w)/p_unseen(w)
      virtual double termWeight(lemur::api::TERMID_T termID, const lemur::api::DocInfo *info) const {
        double sp = seenProb(info->termCount(), termID);
        double usp = unseenCoeff();
        double ref = refLM.prob(termID);
        double score = sp/(usp*ref);
        /*
          cerr << "TW:" << termID << " sp:" << sp << " usp:" << usp << " ref:" << ref << " s:" << score << endl;
        */
        //    return (seenProb(info->termCount(), termID)/(unseenCoeff()* refLM.prob(termID)));
        return score;
      }

      /// doc-specific constant term in the scoring formula
      virtual double scoreConstant() const {
        return unseenCoeff();
      }

      /// a(d)
      virtual double unseenCoeff() const =0; // a(d)
      /// p(w|d), w seen
      virtual double seenProb(double termFreq, lemur::api::TERMID_T termID) const =0;

    protected:
      const lemur::langmod::UnigramLM &refLM;
      const double *docPrMass;
      SimpleKLParameter::SmoothStrategy strategy;
    };



    /// Jelinek-Mercer interpolation 

    /*!

    <PRE>
    P(w|d) = (1-lambda)*Pml(w|d)+ lambda*Pc(w)
    </PRE>
    */

    class JelinekMercerDocModel : public SimpleKLDocModel {
    public:
      JelinekMercerDocModel(lemur::api::DOCID_T docID, 
                            int dl,
                            const lemur::langmod::UnigramLM &collectLM,
                            const double *docProbMass,
                            double collectLMWeight, 
                            SimpleKLParameter::SmoothStrategy smthStrategy=SimpleKLParameter::INTERPOLATE): 
        SimpleKLDocModel(docID, collectLM, dl, docProbMass, smthStrategy),
        lambda(collectLMWeight) {
      };

      virtual ~JelinekMercerDocModel() {};
  
      virtual double unseenCoeff() const {
        if (strategy == SimpleKLParameter::INTERPOLATE) {
          return lambda;
        } else if (strategy==SimpleKLParameter::BACKOFF) {
          return lambda/(1-docPrMass[id]);
        } else {
          throw lemur::api::Exception("JelinekMercerDocModel", "Unknown smoothing strategy");
        }
      }
      virtual double seenProb(double termFreq, lemur::api::TERMID_T termID) const {
        if (strategy == SimpleKLParameter::INTERPOLATE) {
          return ((1-lambda)*termFreq/(double)docLength +
                  lambda*refLM.prob(termID));
        } else if (strategy == SimpleKLParameter::BACKOFF) {
          return ((1-lambda)*termFreq/(double)docLength);
        } else {
          throw lemur::api::Exception("JelinekMercerDocModel", "Unknown smoothing strategy");
        }
      }
    private:
      double lambda;
    };

    /// Bayesian smoothing with Dirichlet prior
    /*!
      <PRE>
      P(w|d) = (c(w;d)+mu*Pc(w))/(|d|+mu)
      </PRE>
    */
    class DirichletPriorDocModel : public SimpleKLDocModel {
    public:
      DirichletPriorDocModel(lemur::api::DOCID_T docID,
                             int dl,
                             const lemur::langmod::UnigramLM &collectLM,
                             const double *docProbMass,
                             double priorWordCount,
                             SimpleKLParameter::SmoothStrategy smthStrategy=SimpleKLParameter::INTERPOLATE): 
        SimpleKLDocModel(docID, collectLM, dl, docProbMass, smthStrategy),
        mu(priorWordCount) {
      };

      virtual ~DirichletPriorDocModel() {};

      virtual double unseenCoeff() const {

        if (strategy == SimpleKLParameter::INTERPOLATE) {
          return mu/(mu+docLength);
        } else if (strategy==SimpleKLParameter::BACKOFF) {
          return (mu/((mu+docLength)*(1-docPrMass[id])));
        } else {
          throw lemur::api::Exception("DirichletPriorDocModel", "Unknown smoothing strategy");
        }
      }

      virtual double seenProb(double termFreq, lemur::api::TERMID_T termID) const {
        if (strategy == SimpleKLParameter::INTERPOLATE) {
          return (termFreq+mu*refLM.prob(termID))/
            (double)(docLength+mu);
        } else if (strategy == SimpleKLParameter::BACKOFF) {
          return (termFreq/(double)(docLength+mu));
        } else {      
          throw lemur::api::Exception("DirichletPriorDocModel", "Unknown smoothing strategy");
        }
      }
    private:
      double mu;
    };

    /// Absolute discout smoothing

    /*!
      P(w|d) = (termFreq - delta)/|d|+ lambda*Pc(w)     if seen
      or = lambda*Pc(w) if unseen
      where, lambda =  unique-term-count-in-d*delta/|d|
    */

    class AbsoluteDiscountDocModel : public SimpleKLDocModel {
    public:
      AbsoluteDiscountDocModel(lemur::api::DOCID_T docID,
                               int dl,
                               const lemur::langmod::UnigramLM &collectLM,
                               const double *docProbMass,
                               lemur::api::COUNT_T *uniqueTermCount,
                               double discount,
                               SimpleKLParameter::SmoothStrategy smthStrategy=SimpleKLParameter::INTERPOLATE): 
        SimpleKLDocModel(docID, collectLM, dl, docProbMass, smthStrategy),
        uniqDocLen(uniqueTermCount),
        delta(discount) {
      };

      virtual ~AbsoluteDiscountDocModel() {};
  
      virtual double unseenCoeff() const {

        if (strategy == SimpleKLParameter::INTERPOLATE) {
          return (delta*uniqDocLen[id]/(double)docLength);
        } else if (strategy==SimpleKLParameter::BACKOFF) {
          return (delta*uniqDocLen[id]/(docLength*(1-docPrMass[id])));
        } else {
          throw lemur::api::Exception("AbsoluteDiscountDocModel", "Unknown smoothing strategy");
        }
      }
      virtual double seenProb(double termFreq, lemur::api::TERMID_T termID) const {
        if (strategy == SimpleKLParameter::INTERPOLATE) {
          return ((termFreq-delta)/(double)docLength+
                  delta*uniqDocLen[id]*refLM.prob(termID)/(double)docLength);
        } else if (strategy == SimpleKLParameter::BACKOFF) {
          return ((termFreq-delta)/(double)docLength);
        } else {
          throw lemur::api::Exception("AbsoluteDiscountDocModel", "Unknown smoothing strategy");
        }
      }
    private:
      double *collectPr;
      lemur::api::COUNT_T *uniqDocLen;
      double delta;
    };


    /// Two stage smoothing : JM + DirichletPrior
    // alpha = (mu+lambda*dLength)/(dLength+mu)
    // pseen(w) = [(1-lambda)*c(w;d)+ (mu+lambda*dLength)*Pc(w)]/(dLength + mu)
    class TwoStageDocModel : public SimpleKLDocModel {
    public:
      TwoStageDocModel(lemur::api::DOCID_T docID,
                       int dl,
                       const lemur::langmod::UnigramLM &collectLM,
                       const double *docProbMass,
                       double firstStageMu, 
                       double secondStageLambda, 
                       SimpleKLParameter::SmoothStrategy smthStrategy=SimpleKLParameter::INTERPOLATE): 
        SimpleKLDocModel(docID, collectLM, dl, docProbMass, smthStrategy),
        mu(firstStageMu),
        lambda(secondStageLambda) {
      };

      virtual ~TwoStageDocModel() {};

      virtual double unseenCoeff() const {

        if (strategy == SimpleKLParameter::INTERPOLATE) {
          return (mu+lambda*docLength)/(mu+docLength);
        } else if (strategy == SimpleKLParameter::BACKOFF) {
          return ((mu+lambda*docLength)/((mu+docLength)*(1-docPrMass[id])));
        } else {
          throw lemur::api::Exception("TwoStageDocModel", "Unknown smoothing strategy");
        }
      }

      virtual double seenProb(double termFreq, lemur::api::TERMID_T termID) const {
        if (strategy == SimpleKLParameter::INTERPOLATE) {      
          return ((1-lambda)*(termFreq+mu*refLM.prob(termID))/
                  (double)(docLength+mu) + lambda*refLM.prob(termID));
        } else if (strategy == SimpleKLParameter::BACKOFF) {
          return (termFreq*(1-lambda)/(double)(docLength+mu));
        } else {
          throw lemur::api::Exception("TwoStageDocModel", "Unknown smoothing strategy");
        }
      }
    private:
      double mu;
      double lambda;
    };
  }
}

#endif /* _SIMPLEKLDOCMODEL_HPP */
