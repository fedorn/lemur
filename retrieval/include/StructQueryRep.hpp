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
/*
  author: fff
*/

#ifndef _STRUCTQUERYREP_HPP
#define _STRUCTQUERYREP_HPP

#include "RetrievalMethod.hpp"
#include "Document.hpp"
#include "DocumentRep.hpp"
#include "StructQryDocRep.hpp"
#include "IndexedReal.hpp"
#include "Index.hpp"
#include "Counter.hpp"
#include "WeightedIDSet.hpp"
#include "FreqVector.hpp"
#include "ScoreAccumulator.hpp"

//------------------------------------------------------------
//      Abstract Interface for Structured Query 
//------------------------------------------------------------

/// A structured query is an adaptor of Document
class StructQuery : public Query {
public:
  StructQuery(Document &doc) : d(doc) {}
  virtual ~StructQuery() {}

  virtual char *id() { return d.getID();}
  virtual void startTermIteration() { d.startTermIteration();}
  virtual bool hasMore() { return d.hasMore();}
  virtual TokenTerm *nextTerm() { return d.nextTerm();}
protected:
  Document &d;
};



//------------------------------------------------------------
//      Abstract Interface for StructQuery Representation 
//------------------------------------------------------------

// forward declaration
class QueryNode;

/// List of query nodes which constitute the one level of a structured
/// query parse tree. Provides iteration and random access of individual 
/// list elements by index.
class QnList {
public:
  QnList() {}
  virtual ~QnList();
  /// initialize for iteration.
  virtual void startIteration() {  i=0; }
  /// are there any more elements in the list.
  virtual bool hasMore() {  return (i < qnList.size()); }
  /// return the next element from the list.
  virtual QueryNode * nextNode(){QueryNode *qn=qnList[i++]; return qn; }
  /// return the i'th element of the list.
  virtual QueryNode * getNode(int i){QueryNode *qn=qnList[i]; return qn; }
  /// return the number of elements in the list.
  virtual int size(){return qnList.size();}
  /// Add a new element to the back of the list.
  virtual void push_back(QueryNode *qn){qnList.push_back(qn);}

protected:
  /// index counter for iteration.
  int i;
  /// the actual vector of QueryNodes.
  vector<QueryNode *> qnList;
};

/// Proximity information for a term within a document. Provides support
/// for ordered and unordered window operators. Used to prune document
/// lists before scoring the documents.
class ProxInfo {
public: 
  ProxInfo() : id(0),count(0),size(0),nextPos(0),posList(NULL),next(NULL) {}
  ~ProxInfo() {
    delete [](posList);
  }
  /// document id
  int id;
  /// term freq
  int count;
  /// window size
  int size;
  /// next position of this term in document
  /// used by unordered window and synonym
  int nextPos;
  /// list of positions in document
  int *posList;
  /// next ProxInfo element in the list.
  ProxInfo *next;
};

/// Base class for all structured query operators.
/// A query node is assumed to have a list of children, an id, a weight
/// (default 1) and an eval function which will be defined by its
/// subclasses
class QueryNode {
public:
  /// New QueryNode with given id and weight.
  QueryNode(int id, double weight) : 
    w(weight),it(id),ch(NULL),entries(0),nextDoc(0),dCnt(0),
    proxList(NULL),dList(NULL) {}
  /// New QueryNode with default id (0) and weight (1).
  QueryNode() : w(1),it(0),ch(NULL),entries(0),nextDoc(0),dCnt(0),
		proxList(NULL),dList(NULL){}
  /// New QueryNode with given weight and default id (0).
  QueryNode(double weight) : w(weight),it(0),ch(NULL),entries(0),
			     nextDoc(0),dCnt(0),proxList(NULL),dList(NULL) {}
  /// clean up internal data.
  ~QueryNode() {
    delete[](dList);
    while(proxList) {
      ProxInfo *prx=proxList->next;
      delete (proxList);
      proxList = prx;
    }
    delete(ch);
  }
  /// the children of this node
  virtual QnList *children() { return ch;}
  /// the nodes id
  virtual int id() { return it;}
  /// evaluate the node on the given document, returning a score.
  virtual double eval(DocumentRep *dRep) = 0;
  /// the weight of this node.
  virtual double weight() { return w; }
  // update weight
  virtual void setWeight(double wt) { w=wt; }
  /// evaluate proximity operator against a document or passage.
  /// Prunes proxList to remove any entries that are no longer
  /// relevant to scoring this operator.
  virtual double proximityScore(DocumentRep *dR, double defaultScore) {
    StructQryDocRep *dRep = (StructQryDocRep *)dR;
    int tf=0;
    double score;
    ProxInfo *prx;
    InvFPIndex &ind = dRep->ind;
    if(dRep->did < nextDoc) {
      return defaultScore;
    }
    while(dRep->did > nextDoc) {
      prx=proxList->next;
      delete (proxList);
      proxList = prx;
      if(!proxList)
	nextDoc = ind.docCount();
      else
	nextDoc = proxList->id;
    }
    if(dRep->did == nextDoc && proxList) {
      double idf = dRep->computeIdfScore(dCnt);
      if(ind.docLength(dRep->did) == dRep->size) {
	// compute proximity score for whole doc
	tf = proxList->count;
      } else {
	// compute proximity score for a passage
	for(int k=0; k<proxList->count; k++)
	  if(proxList->posList[k]> dRep->start && 
	     proxList->posList[k]<=dRep->end)
	    tf++;
      }
      score = dRep->beliefScore(tf, idf);
      prx = proxList->next;
      delete (proxList);
      proxList = prx;
      if(!proxList)
	nextDoc = ind.docCount();
      else
	nextDoc = proxList->id;
    } else
      score = defaultScore;
    return score;
  }
  /// number of documents this node matched
  int dCnt;
  /// array of bool matched docs indexed by docid
  bool *dList;
  /// This node's children list
  QnList *ch;
  /// This node's ProxInfo list
  ProxInfo * proxList;
  /// Number of children in ch
  int entries;
  /// the next document this node matched.
  int nextDoc;

protected:
  /// query node id
  int it;
  /// query node weight
  double w; 
};

/// Structured query representation. Converts the textual representation of
/// the structured query into a tree structure of QueryNodes. Constructs the
/// proximity lists and DocInfoLists for the individula nodes, pruning
/// the set of documents that will actually need to be scored.
class StructQueryRep : public QueryRep {
public:
  /// Parse the text representation into a structured query rep  
  StructQueryRep(StructQuery &qry, InvFPIndex &dbIndex, double dbelief=0);

  virtual ~StructQueryRep() {
    delete(TopNode);
  }
  /// Parse the text representation of the children of a query node.
  virtual QnList * getChildren(StructQuery &qry);
  /// Parse the text representation of the children of a weighted query node.  
  virtual QnList * getWeightedChildren(StructQuery &qry);
  /// Parse the text representation of the children of a proximity query node.  
  virtual QnList * getProxChildren(StructQuery &qry);
  /// Parse the text representation of a weighted query node.  
  virtual QueryNode * getQryNode(StructQuery &qry, TokenTerm *tok, double w);
  /// Parse the text representation of a proximity query node.  
  virtual QueryNode * getProxQryNode(StructQuery &qry, TokenTerm *tok);
  /// Get the topnode of the query parse tree.
  virtual QueryNode * topnode() {return TopNode;}
  /// Set the topnode of the query parse tree.
  virtual void setTopnode(QueryNode *qn) {TopNode=qn;}
  /// Copy the DocInfoList for a given node.
  virtual void copyDocList(DocInfoList *dl, QueryNode *qn);
  /// Union the DocInfoLists of a given node
  virtual void unionDocList(QueryNode *qn);
  /// Intersect the DocInfoLists of a given node
  virtual void intersectDocList(QueryNode *qn);
  /// Construct the term offset list of a given node for a given doc id.
  virtual void termOffsetList(QueryNode *qn, int did);
  /// recursively find matching windows for ordered proximity operator.
  virtual bool foundOrderedProx(int bpos, int wsize, QnList *cl, int ith);
  /// Prune proximity list for ordered proximity operator
  virtual void orderedProxList(QueryNode *qn);
  /// Prune proximity list for unordered proximity operators
  virtual void unorderedProxList(QueryNode *qn);
  /// recursively find matching windows for ordered proximity operators.
  virtual bool findUnorderedWin(QueryNode *cqn, QnList *cl, int winSize);
  /// construct the proximity list synonym operator.
  virtual void synonymProxList(QueryNode *qn);

protected:
  /// Top node of the query parse tree.
  QueryNode *TopNode;
  /// stack of query nodes for parsing nested nodes.
  QueryNode *qStack[100];
  /// index of top of qStack.
  int TopqStack;
  /// default weight.
  double dw;
  /// Our index. Must be an InvFPIndex.
  InvFPIndex &ind;
};

/// Implements the SUM query operator. Evaluates to the average of the
/// scores of its child nodes.
class SumQnode : public QueryNode {
public:
  SumQnode(double wt) {}
  virtual ~SumQnode() {}
  /// Score this node as the sum of the scores of its children divided
  /// by the number of children.
  virtual double eval(DocumentRep *dRep) {
    double count=0;
    double sum=0;
    QueryNode *qn;
    ch->startIteration();
    while(ch->hasMore()) {
      qn =ch->nextNode();
      count++;
      sum +=(double) qn->eval(dRep);
    }
    return sum/count;
  }
};

/// Implements the WSUM query operator. Evaluates to the average of the
/// weighted scores of its child nodes.
class WsumQnode : public QueryNode {
public:
  WsumQnode(double w) {}
  virtual ~WsumQnode() {}
  /// Score this node as the sum of the weighted scores of its children 
  /// divided by the sum of the weights.
  virtual double eval(DocumentRep *dRep) {
    double sum=0;
    double total_wt=0;
    QueryNode *qn;
    double wt;
    ch->startIteration();
    while(ch->hasMore()) {
      qn =ch->nextNode();
      wt = qn->weight();
      total_wt +=wt;
      sum += wt * (double) qn->eval(dRep);
    }
    if(total_wt>0) // normalized by the sum of the weights
      sum /= total_wt;
    // the belief is scaled by the weight 'w' of itself.
    return sum;
  }
};


/// Implements the AND query operator. Evaluates to the product of the
/// max of the default score and the score of each of its child nodes.
class AndQnode : public QueryNode {
public:
  AndQnode(double dbelief, double wt) : dw(dbelief), QueryNode(wt) {}
  virtual ~AndQnode() {}
  /// Score this node as the product of the scores of its children. If 
  /// a child's score is less that the default score, the default
  /// score is used in the product instead of that childs score.
  virtual double eval(DocumentRep *dRep) {
    double prod=1;
    QueryNode *qn;
    double wt;
    ch->startIteration();
    while(ch->hasMore()) {
      qn =ch->nextNode();
      wt = (double) qn->eval(dRep);
      if(wt > dw)
	prod *= wt;
      else
	prod *= dw;
    }
    return prod;
  }
protected:
  /// the default score for a node.
  double dw;
};

/// Implements the OR query operator. Evaluates to the 1 - the product
/// of (1 - score) for each of its child nodes whose score is greater
/// than the default score.
class OrQnode : public QueryNode {
public:
  OrQnode(double dbelief, double wt) : dw(dbelief), QueryNode(wt) {}
  virtual ~OrQnode() {}
  /// Score this node as  (1 - the product of (1 - score) for each child 
  /// node whose score is greater than the default weight. 
  virtual double eval(DocumentRep *dRep) {
    double prod=1.0;
    QueryNode *qn;
    double wt;
    ch->startIteration();
    while(ch->hasMore()) {
      qn =ch->nextNode();
      wt = (double) qn->eval(dRep);
      if(wt > dw)
	prod *= (1.0-wt);
    }
    return (1.0-prod);
  }
protected:
  /// the default score for a node.
  double dw;
};

/// Implements the NOT query operator. Evaluates to the 1 - the score 
/// of its child node.
class NotQnode : public QueryNode {
public:
  NotQnode(double dbelief, double wt) : dw(dbelief), QueryNode(wt) {}
  virtual ~NotQnode() {}
  /// Score as 1 - the score of the single child node.
  virtual double eval(DocumentRep *dRep) {
    // inverting the belief in the only child node
    QueryNode *qn;
    ch->startIteration();
    qn =ch->nextNode();
    return (1.0-(double) qn->eval(dRep));
  }
protected:
  /// the default score for a node.
  double dw;
};

/// Implements the MAX query operator. Evaluates to the maximum of
/// the scores of its child nodes and the default score.
class MaxQnode : public QueryNode {
public:
  MaxQnode(double dbelief, double wt) : dw(dbelief), QueryNode(wt) {}
  virtual ~MaxQnode() {}
  /// Score as the maximum of
  /// the scores of its child nodes and the default score.
  virtual double eval(DocumentRep *dRep) {
    double mx=dw;
    QueryNode *qn;
    double wt;
    ch->startIteration();
    while(ch->hasMore()) {
      qn =ch->nextNode();
      wt = (double) qn->eval(dRep);
      if(wt > mx) 
	mx= wt;
    }
    return mx;
  }
protected:
  /// The default score.
  double dw;

};

/// Implements the BAND query operator. This is the Boolean AND operator.
/// Evaluates to the product of the scores of its child nodes if all of their
/// scores exceed the default score, otherwise 0.
class BandQnode : public QueryNode {
public:
  BandQnode(double dbelief, double w) : dw(dbelief), QueryNode(w) {}
  virtual ~BandQnode() {}
  /// Score as the product of the scores of all child nodes. If any
  /// child node returns a score less than or equal to the default score,
  /// return 0.
  virtual double eval(DocumentRep *dRep) {
    double prod=1;
    QueryNode *qn;
    double wt;
    ch->startIteration();
    while(ch->hasMore()) {
      qn =ch->nextNode();
      wt = (double) qn->eval(dRep);
      if(wt > dw) 
	prod *= wt;
      else 
	return 0;
    }
    return prod;
  }
protected:
  /// the default score.
  double dw;
};

/// Implements the BANDNOT query operator. This is the Boolean AND NOT 
/// operator. Evaluates to the score of its first child node if
/// the score of its second child is less than or equal to the 
/// default score, otherwise 0.
class BandNotQnode : public QueryNode {
public:
  BandNotQnode(double dbelief, double wt) : dw(dbelief), QueryNode(wt) {}
  virtual ~BandNotQnode() {}
  /// Score as the score of its first child node if
  /// the score of its second child is less than or equal to the 
  /// default score, otherwise 0.
  virtual double eval(DocumentRep *dRep) {
    double child1Wt;
    double child2Wt;
    // boolean_and_not consider only two children
    QueryNode *qn;
    ch->startIteration();
    qn =ch->nextNode();
    child1Wt = (double) qn->eval(dRep);
    qn =ch->nextNode();
    child2Wt = (double) qn->eval(dRep);
    if(child2Wt > dw)
      return 0;
    else
      return (child1Wt * (1.0-child2Wt));
  }
protected:
  double dw;

};


/// Implements the FILREJ query operator. This is the FILTER REJECT 
/// operator. Evaluates to the score of its first child node if
/// the score of its second child is less than or equal to the 
/// default score, otherwise the default score.
class FiltRejQnode : public QueryNode {
public:
  FiltRejQnode(double dbelief, double wt) : dw(dbelief), QueryNode(wt) {}
  virtual ~FiltRejQnode() {}
  /// Score as the score of its first child node if
  /// the score of its second child is less than or equal to the 
  /// default score and the score of the first child is greater
  /// than the default score. Otherwise, return the default score.
  virtual double eval(DocumentRep *dRep) {
    double child1Wt;
    double child2Wt;
    // filter_reject consider only two children
    QueryNode *qn;
    ch->startIteration();
    qn =ch->nextNode();
    child1Wt = (double) qn->eval(dRep);
    qn =ch->nextNode();
    child2Wt = (double) qn->eval(dRep);
    if(child1Wt > dw && child2Wt <= dw)
      return child1Wt;
    else
      return dw;
  }
protected:
  /// The default score.
  double dw;

};


/// Implements the FILREQ query operator. This is the FILTER REQUIRE
/// operator. Evaluates to the score of its first child node if
/// the scores of both children are greater than the 
/// default score, otherwise the default score.
class FiltReqQnode : public QueryNode {
public:
  FiltReqQnode(double dbelief, double wt) : dw(dbelief), QueryNode(wt) {}
  virtual ~FiltReqQnode() {}
  /// Score as the score of its first child node if
  /// the scores of both children are greater than the 
  /// default score, otherwise the default score.
  virtual double eval(DocumentRep *dRep) {
    double child1Wt;
    double child2Wt;
    // filter_require consider only two children
    QueryNode *qn;
    ch->startIteration();
    qn =ch->nextNode();
    child1Wt = (double) qn->eval(dRep);
    qn =ch->nextNode();
    child2Wt = (double) qn->eval(dRep);
    if(child1Wt > dw && child2Wt > dw)
      return child1Wt;
    else
      return dw;
  }
protected:
  /// The default score.
  double dw;

};

/// Implements a term query node (leaf). 
class TermQnode : public QueryNode {
public:
  TermQnode(int id, double weight) : QueryNode(id, weight) {}
  virtual ~TermQnode() {}
  /// Score as the termWeight of this term in the document.
  virtual double eval(DocumentRep *dRep) { 
    return(dRep->termWeight(it, NULL));
  }
};

/// Implements the ODN operator. This is the ORDERED WINDOW operator.
/// Evaluates to the proximityScore of the document.
class OdnQNode : public QueryNode {
public:
  OdnQNode(int size, double w, double d): winSize(size), dw(d), 
					  QueryNode(w) {}
  virtual ~OdnQNode() {}
  /// Score as the proximityScore for this document representation.
  virtual double eval(DocumentRep *dRep) {
    return proximityScore(dRep, dw);
  }
  /// Window size for the operator.
  int winSize;
protected:
  /// The default score.
  double dw;
};

/// Implements the UWN operator. This is the UNORDERED WINDOW operator.
/// Evaluates to the proximityScore of the document.
class UwnQNode : public QueryNode {
public:
  UwnQNode(int size, double w, double d): winSize(size), dw(d), 
					  QueryNode(w) {}
  virtual ~UwnQNode() {}
  /// Score as the proximityScore for this document representation.
  virtual double eval(DocumentRep *dRep) {
    return proximityScore(dRep, dw);
  }
  /// Window size for the operator.
  int winSize;
protected:
  /// default score.
  double dw;
};


/// Implements the PASSAGEN operator. 
/// Evaluates to the maximum of the passage scores for the document 
/// normalized by the window size..
class PassageQNode : public QueryNode {
public:
  PassageQNode(int size, double w) : winSize(size), QueryNode(w) {}
  virtual ~PassageQNode() {}
  /// Score as the maximum passage score for the document rep normalized
  /// by window size. The StructQueryDocRep provides passage iteration
  /// over the overlapping windows. The score for an individual passage
  /// is the sum of the scores of the children nodes.
  virtual double eval(DocumentRep *dR) {
    StructQryDocRep *dRep= (StructQryDocRep *)dR;
    int dl = dRep->docEnd;
    dRep->increment = winSize/2;
    dRep->startPassageIteration();
    while(dRep->hasMorePassage()) {
      StructQryDocRep *psgRep= 
	new StructQryDocRep(dRep->did,dRep->ind,dRep->idf,
			    dRep->start,dRep->end);
      double score=0;
      QueryNode *child;
      ch->startIteration();
      while(ch->hasMore()) {
	child =ch->nextNode();
	score +=(double) child->eval(psgRep);
      }
      delete psgRep;
      if(score > dRep->maxScore) {
	dRep->maxScore=score;
	dRep->offset=dRep->start;
      }
      dRep->nextPassage();
    }
    return dRep->maxScore/(double)winSize;
  }
protected:
  /// Passage window size.
  int winSize;
};


/// Implements the SYN operator. This is the SYNONYM operator. Evaluates to 
/// the proximityScore for the document.
class SynQNode : public QueryNode {
public:
  SynQNode(double w, double d) : dw(d), QueryNode(w) {}
  virtual ~SynQNode() {}
  /// Score as the proximityScore for the document.
  virtual double eval(DocumentRep *dRep) {
    return proximityScore(dRep, dw);
  }
protected:
  /// The default score.
  double dw;
};

#endif /* _STRUCTQUERYREP_HPP */

