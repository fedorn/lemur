/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _XLINGDOCMODEL_HPP
#define _XLINGDOCMODEL_HPP

#include "RetrievalMethod.hpp"
#include "Index.hpp"
#include "DocumentRep.hpp"

/*! \brief Doc representation for cross lingual retrieval model.
 */
class XLingDocModel : public DocumentRep {
public:
  /// \brief Constructor taking optional parameters for smoothing.
  /// @param docID internal document id for this document.
  /// @param ind pointer to Index for the document.
  /// @param l lambda value to use for smoothing term weights.
  /// @param nt |V| (sM false) or docCount(t)/Sum_w_in_V(docCount(w)) (sM true)
  /// @param sM whether to use term frequency (tf/|V|) or term doc frequency (docCount(t)/Sum_w_in_V(docCount(w))) for the background model. Default is term frequency.
  XLingDocModel(DOCID_T docID, const Index *ind, double l, double nt, 
		bool sM = false) :
    DocumentRep(docID, ind->docLength(docID)), refIndex(ind), lambda(l), 
				 numTerms(nt),  docBasedSmooth(sM) {
  };
  
  ~XLingDocModel() {};
  /// \brief Computes term weight for a given term
  /// @param termID the term
  /// @param info the DocInfo for this document
  /// @return the smoothed term weight for the term using lambda*(dtf/|D|) + (1-lambda)*P(t|C) where P(t|C) is either term frequency (tf/|V|) or term doc frequency (docCount(t)/Sum_w_in_V(docCount(w))).
  virtual double termWeight(TERMID_T termID, const DocInfo *info) const {
    //    double p_doc = info->termCount()/(double)refIndex->docLength(id);
    double p_doc = info->termCount()/(double)docLength;
    double p_gen; 
    if (docBasedSmooth) 
      p_gen = refIndex->docCount(termID)/numTerms;
    else 
      p_gen = refIndex->termCount(termID)/numTerms;
    return ((lambda * p_doc) + ((1 - lambda) * p_gen));
  }

  /// Unused.
  /// @return lambda
  virtual double scoreConstant() const {
    return lambda;
  }

protected:
  /// Index for the collection
  const Index *refIndex;
  /// denominator (number of terms) for background model probability estimate.
  double numTerms;
  /// smoothing coefficient
  double lambda;
  /// Use term frequency (tf/|V|) when false, use term doc frequency (docCount(t)/Sum_w_in_V(docCount(w))) when true for the background model.
  bool docBasedSmooth;
};

#endif /* _XLINGDOCMODEL_HPP */
