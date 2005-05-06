/*==========================================================================
 *
 *  Original source copyright (c) 2001, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *  Modifications copyright (c) 2002, University of Massachusetts.
 *  See copyright.umass for details.
 *
 *==========================================================================
 */


#ifndef _XLINGRETMETHOD_HPP
#define _XLINGRETMETHOD_HPP

#include "common_headers.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include "IndexTypes.hpp"
#include "FreqVector.hpp"
#include "UnigramLM.hpp"
#include "ScoreFunction.hpp"
#include "XLingDocModel.hpp"
#include "TextQueryRep.hpp"
#include "TextQueryRetMethod.hpp"
#include "Counter.hpp"
#include "DocUnigramCounter.hpp"
#include "PDict.hpp"
#include "TextHandlerManager.hpp"

namespace lemur 
{
  namespace retrieval 
  {
    /// Query term for cross-lingual retrieval queries. 
    class XLQueryTerm : public lemur::api::QueryTerm {
    public:
      XLQueryTerm(lemur::api::TERMID_T tid, double  wt, const char *term, double pge,
                  lemur::dictionary::PDict &dic, lemur::api::Stemmer *stm = NULL) :
        lemur::api::QueryTerm(tid, wt), source(term), p_s_ge(pge), dict(dic),
        stemmer(stm) {
      }

      XLQueryTerm(const char *term, lemur::dictionary::PDict &dic, lemur::api::Stemmer *stm = NULL) : 
        lemur::api::QueryTerm(0, 0), source(term), p_s_ge(0), dict(dic), stemmer(stm) {
      }

      XLQueryTerm(const XLQueryTerm &other) : lemur::api::QueryTerm(0,0), dict(other.dict) {
        ti = other.ti;
        w = other.w;
        p_s_ge = other.p_s_ge;
        source = other.source;
        stemmer = other.stemmer;
      }


      virtual ~XLQueryTerm() { }

      /// accessor for source term
      const string &getSource() const {return source;}

      /// P(s|GE)
      const double getP_s_GE() const {
        return p_s_ge;
      }
  
      /// update weight for this entry.
      void setWeight(double wt) {
        w = wt;
      }

      /// update weight for this entry.
      void incWeight(double wt) {
        w += wt;
      }
      /// equality on the source string.
      virtual bool operator==(const XLQueryTerm& other) const {
        return (other.source == source);
      }

      /// equality on the source string.
      virtual XLQueryTerm& operator=(const XLQueryTerm& other)  {
        ti = other.ti;
        w = other.w;
        p_s_ge = other.p_s_ge;
        source = other.source;
        dict = other.dict;
        stemmer = other.stemmer;
        return (*this);
      }
      /// get translations for source term. 
      /// optionally stems, put others here (transliteration).
      /// caller deletes
      /// if stemmer is not null and not xlates found, stem and try again.
      lemur::dictionary::DictEntryVector *getTranslations() const {
        lemur::dictionary::DictEntryVector *xlates = dict.getTranslations(source);
        // If no xlates, Leah's version stems the term and tries again.
        if (xlates == NULL && stemmer != NULL) {
          // porter stemmer is destructive
          char tmpTerm[512];
          strcpy(tmpTerm, source.c_str());
          string stem = stemmer->stemWord(tmpTerm);
          cerr << "getTranslations: stemming " << source << " to " << stem 
               << endl;
          xlates = dict.getTranslations(stem);
        }
        return xlates;
      }
    private:
      string source;
      double p_s_ge;
      lemur::dictionary::PDict &dict;  
      lemur::api::Stemmer *stemmer;
    };


    /// Query model representation for the cross lingual retrieval model
    class XLingQueryModel : public lemur::api::QueryRep {
    public:
      /// \brief construct a query model based on query text in the source language.
      /// @param qry the TextQuery in the source language.
      /// @param source the Index for the source language collection.
      /// @param dbS whether or not to use document frequency based smoothing.
      /// @param numSource the number of terms in the source language collection.
      /// @param dict the translation dictionary to use.
      /// @param stp optional Stopper for source language terms.
      /// @param stm optional Stemmer for source language terms.
      XLingQueryModel(const lemur::api::TermQuery &qry, 
                      const lemur::api::Index &source, 
                      bool dbS, double numSource,
                      lemur::dictionary::PDict &dict, 
                      const lemur::api::Stopper *stp = NULL, 
                      lemur::api::Stemmer *stm = NULL) {
        // fill in weighted terms
        // P(e|GE)
        double pge;
        numTerms = 0;
    
        qry.startTermIteration();
        while (qry.hasMore()) {
          const lemur::api::Term *t = qry.nextTerm();
          // if Stopper is not NULL, test for stopwords.
          if (stp == NULL || !(stp->stopWord(t->spelling()))) {
            numTerms++;
            XLQueryTerm st(t->spelling(), dict, stm);
            iter = find(qTerms.begin(), qTerms.end(), st);
            if (iter != qTerms.end()) {
              // found it, bump count
              (*iter).incWeight(1);
            } else {
              // new term
              lemur::api::TERMID_T ti = source.term(t->spelling());
              if (ti>0) {
                // pge
                if (dbS) {
                  pge = source.docCount(ti)/numSource;
                } else {
                  pge = (source.termCount(ti)/numSource);      
                }
              } else {
                // OOV, use default pge
                // perhaps this would be better estimated with:
                //        pge = 1/(numSource + 1);
                pge = (0.000001*0.000001);
              }
              XLQueryTerm newTerm(ti, 1, t->spelling(), pge, dict, stm);
              qTerms.push_back(newTerm);
            }
          } else {
            cerr << "XLingQueryModel: " << t->spelling() 
                 << " on stoplist, ignoring" << endl;
          }
      
        }
      }
  
      virtual ~XLingQueryModel() {
      }

      /// Begin iteration over the XLQueryTerms
      virtual void startIteration() const {
        iter = qTerms.begin();
      }
      /// Does the query have more terms?
      virtual bool hasMore() const {
        return (iter != qTerms.end());
      }
      /// Get next XLQueryTerm from the query.
      //  virtual XLQueryTerm &nextTerm() {
      virtual XLQueryTerm &nextTerm() const {
        return (*iter++);
      }
      virtual int getNumTerms() const {return numTerms;}
  
    private:
      mutable vector<XLQueryTerm> qTerms;
      mutable vector<XLQueryTerm>::iterator iter;
      int numTerms;
    };

    // Should not really be a TextQueryRetMethod, as it does not score
    // in a like fashion. but does take advantage of the cached doc reps.
    //
    /*! \brief Cross lingual retrieval method. */
    /*!
      Translation dictionary based retrieval, scoring queries in the source
      language against documents in the target language using:<br>
      P(Q_s|D_t) = Prod_w_in_Q_s(lambda(Sum_t_in_D_t P(t|D_t)P(w|t) + (1-lambda)P(w|G_s)<br>
      where G_s is the background model for the source language.
    */
    //class XLingRetMethod : public TextQueryRetMethod {
    class XLingRetMethod : public lemur::api::RetrievalMethod {
    public:

      /// \brief Constructor
      /// @param dbIndex index for target language documents
      /// @param background index for source language background model
      /// @param dict PDict containing source->target translation probabilities
      /// @param accumulator ScoreAccumulator for intermediate results
      /// @param l lambda value to use for smoothing background model
      /// @param b beta value to use for smoothing P(t|D)
      /// @param cacheDR whether or not to cache document reps
      /// @param sBM whether to use term frequency (tf/|V|) or term doc frequency (docCount(t)/Sum_w_in_V(docCount(w))) for the source language background model. Default is term frequency.
      /// @param tBM whether to use term frequency (tf/|V|) or term doc frequency (docCount(t)/Sum_w_in_V(docCount(w))) for the targetlanguage background model. Default is term frequency.
      /// @param stp source language Stopper to use when getting translations.
      /// @param stm source language Stemmer to use when getting translations.
      XLingRetMethod(const lemur::api::Index &dbIndex, 
                     const lemur::api::Index &background, 
                     lemur::dictionary::PDict &dict, 
                     lemur::api::ScoreAccumulator &accumulator, 
                     double l, double b, bool cacheDR,
                     string &sBM, string &tBM, 
                     const lemur::api::Stopper *stp = NULL, 
                     lemur::api::Stemmer *stm = NULL);
      /// clean up.
      virtual ~XLingRetMethod();
  
      /// \brief Create a document representation
      /// @param docID the internal document id to create the representation for
      /// @return An instance of XLingDocRep
      virtual lemur::api::DocumentRep *computeDocRep(lemur::api::DOCID_T docID);

      /// \brief Score a given term for a given document
      /// @param id the term id
      /// @param weight the weight for this term
      /// @param info the DocInfo for this document
      /// @param dRep the DocumentRep for this document
      /// @return P(t|D) * P(s|t)
      virtual double matchedTermWeight(lemur::api::TERMID_T id, double weight,
                                       const lemur::api::DocInfo *info, 
                                       const lemur::api::DocumentRep *dRep) const { 
        double d = dRep->termWeight(id,info); //P(a|D)
        double score = d * weight; //P(a|D) * P(e|a)
        return score;
      }

      /// \brief Adjust the score for a given document
      /// @param origScore the original score
      /// @param pge the background probability to adjust by.
      /// @return log((lambda * origScore) + ((1 - lambda) * pge))
      virtual double adjustedScore(double origScore, double pge) const {
        return (log((lambda * origScore) + ((1 - lambda) * pge)));
      }

      virtual void scoreCollection(const lemur::api::QueryRep &qry, 
                                   lemur::api::IndexedRealVector &results){
        scoreInvertedIndex(qry, results);
      }
      // Override (have to do individual doc ones too.
      virtual void scoreInvertedIndex(const lemur::api::QueryRep &qryRep, 
                                      lemur::api::IndexedRealVector &scores, 
                                      bool scoreAll = false);

      virtual lemur::api::QueryRep *computeQueryRep(const lemur::api::Query &qry) {
        if (const lemur::api::TermQuery *q = dynamic_cast<const lemur::api::TermQuery *>(&qry))
          return (new XLingQueryModel(*q, source, docBasedSourceSmooth, numSource,
                                      dictionary, stopper, stemmer));
        else LEMUR_THROW(LEMUR_RUNTIME_ERROR, "XLingRetMethod expects query of type TermQuery");
      } 

      virtual lemur::api::QueryRep *computeTargetKLRep(const lemur::api::QueryRep *qry);

      /// Score a document identified by the id w.r.t. a query rep
      virtual double scoreDoc(const lemur::api::QueryRep &qry, lemur::api::DOCID_T docID);

      /// update the query -- noop
      virtual void updateQuery(lemur::api::QueryRep &qryRep, const lemur::api::DocIDSet &relDocs) {}

    protected:
      virtual double scoreDocVector(const XLingQueryModel &qRep, lemur::api::DOCID_T docID, 
                                    lemur::utility::FreqVector &docVector);

      double lambda;
      double beta;
      double numSource;
      double numTarget;
      bool docBasedSourceSmooth;
      bool docBasedTargetSmooth;
      lemur::api::ScoreAccumulator &scAcc; // this does not need to be passed in. Bleah.
      lemur::dictionary::PDict &dictionary;
      lemur::api::Stemmer *stemmer; // source language
      const lemur::api::Stopper *stopper; // source language
      const lemur::api::Index &source;
      /// cache document reps.
      lemur::api::DocumentRep **docReps;
      /// whether or not to cache document representations
      bool cacheDocReps;
      /// number of documents plus 1, the size of the docReps array.
      int docRepsSize;
      lemur::api::ScoreAccumulator *termScores;
    };
  }
}
#endif /* _XLINGRETMETHOD_HPP */
