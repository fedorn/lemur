/*==========================================================================
 * Copyright (c) 2002-2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
  author: fff, dmf
  * 03/31/2003 -- Split QueryNode out of StructQueryRep, refactor some 
  * code out of rep into node classes (more OOP that way).
  */

#ifndef _QUERYNODE_HPP
#define _QUERYNODE_HPP
#include "StructQryDocRep.hpp"
#include "ProxInfo.hpp"
namespace lemur 
{
  namespace retrieval 
  {
    
    // forward declaration
    class QueryNode;

    /// \brief List of query nodes which constitute the one level of a structured query parse tree. 
    /*! Provides iteration and random access of individual list elements 
      by index.
    */
    class QnList {
    public:
      QnList() {}
      ~QnList(); 
      /// initialize for iteration.
      void startIteration() {i = 0;}
      /// are there any more elements in the list.
      bool hasMore() {  return (i < qnList.size()); }
      /// return the next element from the list.
      QueryNode * nextNode()  {QueryNode *qn = qnList[i++]; return qn; }
      /// return the i'th element of the list.
      const QueryNode * getNode(int j) const{QueryNode *qn = qnList[j]; return qn; }
      /// return the number of elements in the list.
      int size() const{return qnList.size();}
      /// Add a new element to the back of the list.
      void push_back(QueryNode *qn){qnList.push_back(qn);}
      /// remove an elt and return it (allows moving from list to list).
      QueryNode *popNode() { 
        QueryNode *qn = qnList[i]; 
        qnList[i++] = NULL;
        return qn;  
      }
    private:
      /// index counter for iteration.
      mutable int i;
      /// the actual vector of QueryNodes.
      vector<QueryNode *> qnList;
    };
    //------------------------------------------------------------
    //      Abstract Interface for Query Nodes 
    //------------------------------------------------------------


    /// \brief Base class for all structured query operators.
    /*!
      A query node is assumed to have a list of children, an id, a weight
      (default 1) and an eval function which will be defined by its
      subclasses.
    */
    class QueryNode {
    public:
      /// New QueryNode with given id and weight.
      QueryNode(int id, double weight) : 
        w(weight),it(id),ch(NULL),entries(0),nextDoc(0),dCnt(0),
        dList(NULL), proxList(NULL), dw(0) {}
      /// New QueryNode with default id (0) and weight (1).
      QueryNode() : w(1),it(0),ch(NULL),entries(0),nextDoc(0),dCnt(0),
                    dList(NULL), proxList(NULL), dw(0){}
      /// New QueryNode with given weight and default id (0).
      QueryNode(double weight, double dWeight = 0) : w(weight), it(0), ch(NULL),
                                                     entries(0), nextDoc(0),
                                                     dCnt(0),proxList(NULL),
                                                     dList(NULL), 
                                                     dw(dWeight) {}
      /// clean up internal data.
      virtual ~QueryNode() {
        delete[](dList);
        delete(proxList);
        delete(ch);
      }
      /// the children of this node
      const QnList *children() const { return ch;}
      /// set the children of this node
      void setChildren(QnList *cl) {ch = cl;}
      /// the node's id
      int id() const { return it;}
      /// the weight of this node.
      double weight() const { return w; }
      // update weight
      void setWeight(double wt) { w = wt; }
      // update entries count
      void setEntries(int cnt) { entries = cnt; }

      /// Copy the DocInfoList for a given node.
      virtual void copyDocList(int len, int tf, const lemur::api::DocInfoList *dl, int dc) {
      }
      /// update doc list from children's doc lists.
      virtual void updateDocList(int numDocs) = 0;
      /// evaluate the node on the given document, returning a score.
      virtual double eval(const lemur::api::DocumentRep *dRep) const = 0;

      /// Does this node have a proxList?
      bool isProxNode() const { return proxList != NULL; }
      /// Iteration through a node's prox lists.
      void startProxIteration() const {if (proxList) proxList->startIteration();}
      /// Any entries left in the list?
      bool hasMoreProx() const {return proxList && proxList->hasMore();}
      /// advance to next element in list. return false if empty.
      bool nextProxItem() const {return proxList && proxList->nextDoc();}
      /// advance to element with id did, or to next element beyond if
      /// did is not found. Return true if found, false if not.
      bool nextProxItem(lemur::api::DOCID_T did) const{
        return proxList && proxList->nextDoc(did);
      }

      /// number of documents this node matched
      int dCnt;
      /// array of bool matched docs indexed by docid
      bool *dList;
      /// the next document this node matched.
      mutable lemur::api::DOCID_T nextDoc;
      /// This node's ProxInfo list
      ProxInfo * proxList;

    protected:  
      /// flatten children of a node into weighted proximity entries.
      /// removes all belief operators.
      void transformPassageOps();
      /// This node's children list
      QnList *ch;
      /// Number of children in ch
      int entries;
      /// query node id
      int it;
      /// query node weight
      double w; 
      /// The default score.
      double dw;

      /// Union the DocInfoLists of a given node
      void unionDocList(int numDocs);
      /// Intersect the DocInfoLists of a given node
      void intersectDocList(int numDocs);
    };

    /// Parent class for all belief operators
    class BeliefNode : public QueryNode {
    public:
      BeliefNode(double wt) : QueryNode(wt) { }
      BeliefNode(int id, double weight) : QueryNode(id, weight) { }  
      BeliefNode(double wt, double dbelief) : QueryNode(wt, dbelief) { }
      virtual ~BeliefNode() { }
      /// Union the doc lists of the children of this node.
      virtual void updateDocList(int numDocs) {
        unionDocList(numDocs);   
      }
    };


    /// Parent class for all prox operators
    class ProxNode : public QueryNode {
    public:
      ProxNode(double wt) : QueryNode(wt) {}
  
      ProxNode(int id, double weight) :  QueryNode(id, weight) { }  
      ProxNode(double w, double d) : QueryNode(w, d), winSize(0) {}
      ProxNode(int size, double w, double d) : QueryNode(w, d), winSize(size) {}
      virtual ~ProxNode() { }
      /// Score as the proximityScore for this document representation.
      virtual double eval(const lemur::api::DocumentRep *dRep) const {
        return proximityScore(dRep, dw);
      }
      /// Intersect the doc lists of the children of this node.
      virtual void updateDocList(int numDocs) {
        intersectDocList(numDocs);   
      }

    protected:
      /// Size of the window for this node.
      int winSize;
    private:
      /// evaluate proximity operator against a document
      /// Prunes proxList to remove any entries that are no longer
      /// relevant to scoring this operator.
      double proximityScore(const lemur::api::DocumentRep *dR, double defaultScore) const {
        const StructQryDocRep *dRep = (const StructQryDocRep *)dR;
        int tf = 0;
        double score;
        if(dRep->did < nextDoc) {
          return defaultScore;
        }
        // Skip to next doc id that we can score (> or == to current).
        if (proxList->nextDoc(dRep->did)) {
          double idf = dRep->computeIdfScore(dCnt);
          // compute proximity score for whole doc
          tf = proxList->count();
          score = dRep->beliefScore(tf, idf);
          // advance the prox pointer.
          if (proxList->nextDoc()) {
            nextDoc = proxList->id();
          } else {
            // No more prox entries.
            nextDoc = INT_MAX;
          }
        } else {
          score = defaultScore;
        }
        return score;
      }
    };


    /// \brief Implements the SUM query operator. Evaluates to the 
    /// average of the scores of its child nodes.
    class SumQnode : public BeliefNode {
    public:
      SumQnode(double wt) : BeliefNode(wt){}
      virtual ~SumQnode() {}
      /// Score this node as the sum of the scores of its children divided
      /// by the number of children.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double sum = 0;
        int count = 0;
        const QueryNode *qn;
        ch->startIteration();
        while(ch->hasMore()) {
          qn = ch->nextNode();
          sum += qn->eval(dRep);
          count++;
        }
        //    return sum/entries;
        return sum/count;
      }
    };

    /// \brief Implements the WSUM query operator. 
    /// Evaluates to the average of the weighted scores of its child nodes.
    class WsumQnode : public BeliefNode {
    public:
      WsumQnode(double w) : BeliefNode(w) {}
      virtual ~WsumQnode() {}
      /// Score this node as the sum of the weighted scores of its children 
      /// divided by the sum of the weights.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double sum = 0;
        double total_wt = 0;
        const QueryNode *qn;
        double wt;
        ch->startIteration();
        while(ch->hasMore()) {
          qn = ch->nextNode();
          wt = qn->weight(); 
          total_wt += wt;
          sum += wt * qn->eval(dRep);
        }
        if(total_wt > 0) // normalized by the sum of the weights
          sum /= total_wt;
        // the belief is scaled by the weight 'w' of itself.
        return sum;
      }
    };


    /// \brief Implements the AND query operator. 
    /// Evaluates to the product of the
    /// max of the default score and the score of each of its child nodes.
    class AndQnode : public BeliefNode {
    public:
      AndQnode(double dbelief, double wt) : BeliefNode(wt, dbelief) {}
      virtual ~AndQnode() {}
      /// Score this node as the product of the scores of its children. If 
      /// a child's score is less that the default score, the default
      /// score is used in the product instead of that childs score.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double prod = 1;
        const QueryNode *qn;
        double wt;
        ch->startIteration();
        while(ch->hasMore()) {
          qn = ch->nextNode();
          wt = qn->eval(dRep);
          if(wt > dw)
            prod *= wt;
          else
            prod *= dw;
        }
        return prod;
      }
    };

    /// \brief Implements the OR query operator. 
    /// Evaluates to the 1 - the product
    /// of (1 - score) for each of its child nodes whose score is greater
    /// than the default score.
    class OrQnode : public BeliefNode {
    public:
      OrQnode(double dbelief, double wt) : BeliefNode(wt, dbelief) {}
      virtual ~OrQnode() {}
      /// Score this node as  (1 - the product of (1 - score) for each child 
      /// node whose score is greater than the default weight. 
      virtual double eval(const lemur::api::DocumentRep *dRep) const {
        double prod = 1.0;
        const QueryNode *qn;
        double wt;
        ch->startIteration();
        while(ch->hasMore()) {
          qn = ch->nextNode();
          wt = qn->eval(dRep);
          if(wt > dw)
            prod *= (1.0 - wt);
        }
        return (1.0 - prod);
      }
    };

    /// \brief Implements the NOT query operator. 
    /// Evaluates to the 1 - the score 
    /// of its child node.
    class NotQnode : public BeliefNode {
    public:
      NotQnode(double dbelief, double wt) : BeliefNode(wt, dbelief) {}
      virtual ~NotQnode() {}
      /// Score as 1 - the score of the single child node.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        // inverting the belief in the only child node
        const QueryNode *qn;
        ch->startIteration();
        qn = ch->nextNode();
        return (1.0 - qn->eval(dRep));
      }
    };

    /// \brief Implements the MAX query operator. Evaluates to the maximum of
    /// the scores of its child nodes and the default score.
    class MaxQnode : public BeliefNode {
    public:
      MaxQnode(double dbelief, double wt) : BeliefNode(wt, dbelief) {}
      virtual ~MaxQnode() {}
      /// Score as the maximum of
      /// the scores of its child nodes and the default score.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double mx = dw;
        const QueryNode *qn;
        double wt;
        ch->startIteration();
        while(ch->hasMore()) {
          qn = ch->nextNode();
          wt = qn->eval(dRep);
          if(wt > mx) 
            mx = wt;
        }
        return mx;
      }
    };

    /// \brief Implements the BAND query operator. 
    /// This is the Boolean AND operator.
    /// Evaluates to the product of the scores of its child nodes if all 
    /// of their scores exceed the default score, otherwise 0.
    class BandQnode : public BeliefNode {
    public:
      BandQnode(double dbelief, double w) : BeliefNode(w, dbelief) {}
      virtual ~BandQnode() {}
      /// Score as the product of the scores of all child nodes. If any
      /// child node returns a score less than or equal to the default score,
      /// return 0.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double prod = 1.0;
        const QueryNode *qn;
        double wt;
        StructQryDocRep * myRep = (StructQryDocRep *)dRep;
        lemur::api::DOCID_T did = myRep->did;
        ch->startIteration();
        while(ch->hasMore()) {
          qn = ch->nextNode();
          // advance child prox entry to this document
          if (qn->hasMoreProx() && qn->nextProxItem(did))
            qn->nextDoc = did; // update nextDoc for term node eval
          wt = qn->eval(dRep);
          if(wt > dw) 
            prod *= wt;
          else 
            return 0;
        }
        return prod;
      }
      virtual void updateDocList(int numDocs) {
        intersectDocList(numDocs); // different from superclass.
      }
    };

    /// \brief Implements the BANDNOT query operator. 
    /// This is the Boolean AND NOT 
    /// operator. Evaluates to the score of its first child node if
    /// the score of its second child is less than or equal to the 
    /// default score, otherwise 0.
    class BandNotQnode : public BeliefNode {
    public:
      BandNotQnode(double dbelief, double wt) : BeliefNode(wt, dbelief) {}
      virtual ~BandNotQnode() {}
      /// Score as the score of its first child node if
      /// the score of its second child is less than or equal to the 
      /// default score, otherwise 0.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double child1Wt;
        double child2Wt;
        // boolean_and_not consider only two children
        const QueryNode *qn;
        ch->startIteration();
        qn = ch->nextNode();
        child1Wt = qn->eval(dRep);
        qn = ch->nextNode();
        child2Wt = qn->eval(dRep);
        if(child2Wt > dw)
          return 0;
        else
          return (child1Wt * (1.0 - child2Wt));
      }
    };


    /// \brief Implements the FILREJ query operator. This is the FILTER REJECT 
    /// operator. Evaluates to the score of its first child node if
    /// the score of its second child is less than or equal to the 
    /// default score, otherwise the default score.
    class FiltRejQnode : public BeliefNode {
    public:
      FiltRejQnode(double dbelief, double wt) : BeliefNode(wt, dbelief) {}
      virtual ~FiltRejQnode() {}
      /// Score as the score of its first child node if
      /// the score of its second child is less than or equal to the 
      /// default score and the score of the first child is greater
      /// than the default score. Otherwise, return the default score.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double child1Wt;
        double child2Wt;
        // filter_reject consider only two children
        const QueryNode *qn;
        ch->startIteration();
        qn = ch->nextNode();
        child1Wt = qn->eval(dRep);
        qn = ch->nextNode();
        child2Wt = qn->eval(dRep);
        if(child1Wt > dw && child2Wt <= dw)
          return child1Wt;
        else
          return dw;
      }
    };

    /// \brief Implements the FILREQ query operator. This is the FILTER REQUIRE
    /// operator. Evaluates to the score of its first child node if
    /// the scores of both children are greater than the 
    /// default score, otherwise the default score.
    class FiltReqQnode : public BeliefNode {
    public:
      FiltReqQnode(double dbelief, double wt) : BeliefNode(wt, dbelief) {}
      virtual ~FiltReqQnode() {}
      /// Score as the score of its first child node if
      /// the scores of both children are greater than the 
      /// default score, otherwise the default score.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{
        double child1Wt;
        double child2Wt;
        // filter_require consider only two children
        const QueryNode *qn;
        ch->startIteration();
        qn = ch->nextNode();
        child1Wt = qn->eval(dRep);
        qn = ch->nextNode();
        child2Wt = qn->eval(dRep);
        if(child1Wt > dw && child2Wt > dw)
          return child1Wt;
        else
          return dw;
      }
      virtual void updateDocList(int numDocs) {
        intersectDocList(numDocs); // different from superclass.
      }
    };

    /// Implements a term query node (leaf). 
    class TermQnode : public ProxNode {
    public:
      TermQnode(int id, double weight) : ProxNode(id, weight) { }
      virtual ~TermQnode() {}
      /// Score as the termWeight of this term in the document.
      virtual double eval(const lemur::api::DocumentRep *dRep) const{ 
        StructQryDocRep * myRep = (StructQryDocRep *)dRep;
        lemur::api::DOCID_T did = myRep->did;
        double freq = 0.0;
        if (nextDoc == did) {
          freq = (double)proxList->count();
          if (hasMoreProx()) {
            // advance the prox pointer.
            nextProxItem();
            nextDoc = proxList->id();
          } else {
            // No more prox entries.
            nextDoc = INT_MAX;
          }
        }
        return(myRep->termWeight(it, freq, dCnt));
      }
      /// Copy the DocInfoList for a given node.
      virtual void copyDocList(int len, int tf, const lemur::api::DocInfoList *dl, int dc);
    };

    /// \brief Implements the ODN operator. This is the ORDERED WINDOW operator.
    /// Evaluates to the proximityScore of the document.
    class OdnQNode : public ProxNode {
    public:
      OdnQNode(int size, double w, double d) : ProxNode(size, w, d){}
      virtual ~OdnQNode() {}
      virtual void updateDocList(int numDocs) {
        ProxNode::updateDocList(numDocs);   // use superclass method
        orderedProxList(numDocs);
      }
  
    private:
      /// Prune proximity list for ordered proximity operator
      void orderedProxList(int numDocs);
      /// recursively find matching windows for ordered proximity operator.
      bool foundOrderedProx(int bpos, int wsize, const QnList *cl, int ith);
    };

    /// \brief Implements the UWN operator. This is the UNORDERED WINDOW operator.
    /// Evaluates to the proximityScore of the document.
    class UwnQNode : public ProxNode {
    public:
      UwnQNode(int size, double w, double d) : ProxNode(size, w, d) {}
      virtual ~UwnQNode() {}
      virtual void updateDocList(int numDocs) {
        ProxNode::updateDocList(numDocs);   // use superclass method
        unorderedProxList(numDocs);
      }
    private:
      /// Prune proximity list for unordered proximity operator
      void unorderedProxList(int numDocs);
      /// find matching windows for unordered proximity operators.
      bool findUnorderedWin(const QueryNode *cqn, QnList *cl, int winSize);
    };


    /// \brief Implements the PASSAGEN operator. 
    // fix to be WPARSUMN 
    /// This is a prox operator with embedded belief operators spliced out.
    /// Evaluates to the maximum of the passage scores for the document 
    /// normalized by the window size..
    class PassageQNode : public ProxNode {
    public:
      PassageQNode(int size, double w) : ProxNode(w){ winSize = size; }
      virtual ~PassageQNode() {}

      /// Score as the maximum passage score for the document rep normalized
      /// by window size. The StructQueryDocRep provides passage iteration
      /// over the overlapping windows. The score for an individual passage
      /// is the sum of the scores of the children nodes.
      virtual double eval(const lemur::api::DocumentRep *dR) const{
        StructQryDocRep *dRep = (StructQryDocRep *)dR;
        double maxScore = 0;
        double score;      
        dRep->startPassageIteration(winSize);
        while(dRep->hasMorePassage()) {
          score = passageScore(dRep);
          if(score > maxScore) {
            maxScore = score;
          }
          dRep->nextPassage();
        }
        return maxScore;
      }

      virtual void updateDocList(int numDocs) {
        unionDocList(numDocs); // different from superclass.
        transformPassageOps();
      }
    private:
      /// weighted sum of prox children
      /// all belief operators have already been removed/flattened.
      double passageScore(const StructQryDocRep *dRep) const;
    };

    /// \brief Implements the SYN operator. Evaluates to the proximityScore 
    /// for the conflated child terms within the document.
    class SynQNode : public ProxNode {
    public:
      SynQNode(double w, double d) : ProxNode(w, d) {}
      virtual ~SynQNode() {}
      virtual void updateDocList(int numDocs) {
        unionDocList(numDocs); // different from superclass.
        synonymProxList();
      }
  
    private:
      /// construct the proximity list synonym operator.
      void synonymProxList();
    };

    /// \brief Implements the PROP operator. 
    /// This implementation uses #0 to model
    /// #prop(field proxnode). Implementation and inheritance may change.
    class PropQNode : public OdnQNode {
    public:
      PropQNode(double w, double d) : OdnQNode(0, w, d){}
      virtual ~PropQNode() {}
    };
  }
}

#endif
