/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/

#ifndef _STRUCTQRYDOCREP_HPP
#define _STRUCTQRYDOCREP_HPP

#include "DocumentRep.hpp"
#include "InvFPTermList.hpp"
#include "InvFPIndex.hpp"
#include "Counter.hpp"

/// Document term frequency counter that supports both whole documents and
/// and passages of documents. Requires the use of an InvFPIndex.
class DocTermsCounter : public ArrayCounter <int> {
public:
  /// construct a counter for a doc
  DocTermsCounter(int docID, InvFPIndex &indx) : 
    ind(indx), ArrayCounter<int>(indx.termCountUnique()+1)
  { 
    TermInfoList *tList = indx.termInfoList(docID);
    TermInfo *info;
    tList->startIteration();
    while (tList->hasMore()) {
      info = tList->nextEntry();
      incCount(info->id(), info->count());
    }
    delete tList;
  }
  /// construct a counter for a passage from the document.
  DocTermsCounter(int docID, InvFPIndex &indx, int start, int end) : 
    ind(indx), ArrayCounter<int>(indx.termCountUnique()+1)
  { 
    TermInfoList *dTerms=indx.termInfoListSeq(docID);
    InvFPTerm *term;
    dTerms->startIteration();
    while (dTerms->hasMore()) {
      term = (InvFPTerm *) dTerms->nextEntry();
      if(term->position()>start) {
	if(term->position() > end)
	  break;
	incCount(term->id(), 1);
      }
    }
    delete dTerms;
  }

  virtual ~DocTermsCounter() {}

protected:
  InvFPIndex &ind;
};

/// Representation of a doc (as a weighted vector) in the StructQry method.
/// Provides support for passage level operations on a document.
class StructQryDocRep : public DocumentRep {
public:
  /// New StructQryDocRep for an entire document.
  StructQryDocRep(int docID, InvFPIndex &dbIndex, double *idfValue) :
    DocumentRep(docID), did(docID), ind(dbIndex), idf(idfValue) {
    start=0;
    end=dbIndex.docLength(docID);
    size=end;
    docEnd=end;
    docTermFrq = new DocTermsCounter(docID, dbIndex);
  }

  /// New StructQryDocRep for a passage from a document beginning at bg and
  /// ending and nd.
  StructQryDocRep(int docID, InvFPIndex &dbIndex, double *idfValue, int bg, int nd) :
    DocumentRep(docID), did(docID), ind(dbIndex), idf(idfValue), start(bg), end(nd) {
    size=nd-bg+1;
    docEnd=dbIndex.docLength(docID);
    maxScore=0;
    offset=0;
    docTermFrq = new DocTermsCounter(docID, dbIndex, bg, nd);
  }

  virtual ~StructQryDocRep() { delete docTermFrq; }
  /// Belief score for this term in this document.
  virtual double termWeight(int termID, DocInfo *info);
  /// return 0.
  virtual double scoreConstant() { return 0;}

  virtual void startPassageIteration();
  virtual bool hasMorePassage();
  /// next block of psgSize termids, empty positions == OOV (0);
  virtual void nextPassage();
  /// compute idf for the given document frequency as
  /// log((|C|+0.5)/df)/(log(|C|+1)
  virtual double computeIdfScore(double df);
  /// compute the belief score for a given term frequency.
  virtual double beliefScore(double df, double idf);
  /// The lenght of the current passage.
  virtual double passageLength() {return end-start;};
  /// the document id.
  int did;
  InvFPIndex & ind;

  double *idf;
  /// start position of a passage
  int start; 
  /// end position of a passage
  int end;
  /// size used by passages
  int size; 
  /// passage overlap value. default is size/2.
  int increment; 
  /// length of the document.
  int docEnd;  
  /// record the offset of passage having the maxScore
  int offset;      
  /// record the max score of the passages
  double maxScore; 
  DocTermsCounter *docTermFrq;
};
#endif
