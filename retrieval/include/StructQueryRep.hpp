/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
  author: fff
*/

#ifndef _STRUCTQUERYREP_HPP
#define _STRUCTQUERYREP_HPP

#include "RetrievalMethod.hpp"
#include "TextQuery.hpp"
#include "Index.hpp"
#include "QueryNode.hpp"

namespace lemur 
{
  namespace retrieval
  {
    
    /// \brief Structured query representation. 

    /*! Converts the textual representation of
      the structured query into a tree structure of QueryNodes. Constructs the
      proximity lists and DocInfoLists for the individual nodes, pruning
      the set of documents that will actually need to be scored.
    */
    class StructQueryRep : public lemur::api::QueryRep {
    public:
      /// Parse the text representation into a structured query rep  
      StructQueryRep(const lemur::api::TermQuery &qry, const lemur::api::Index &dbIndex, 
                     double dbelief = 0);

      virtual ~StructQueryRep() {
        delete(topNode);
      }
      /// Get the topnode of the query parse tree.
      /// maybe this method should return a const object so it can't be deleted
      /// but sometimes we want to allow caller to change node's weight.
      virtual QueryNode * topnode() const {return topNode;}
      /// Set the topnode of the query parse tree.
      virtual void setTopnode(QueryNode *qn) {topNode = qn;}
  
    private:
      /// pointer to member function for getting a node.
      typedef QueryNode *(StructQueryRep::*getFunc)(const lemur::api::TermQuery &, const lemur::api::Term *,
                                                    double);
      /// Parse the text representation of the children of a query node.
      QnList * getChildren(const lemur::api::TermQuery &qry, getFunc fn, 
                           bool weigh = false);
      /// Parse the text representation of a weighted query node.  
      QueryNode * getQryNode(const lemur::api::TermQuery &qry, const lemur::api::Term *tok, double w);
      /// Parse the text representation of a proximity query node.  
      QueryNode * getProxQryNode(const lemur::api::TermQuery &qry, const lemur::api::Term *tok,
                                 double w = 1.0);
      /// Top node of the query parse tree.
      QueryNode *topNode;
      /// stack of query nodes for parsing nested nodes.
      QueryNode *qStack[100]; /// This is an accident waiting to happen! dmf
      /// index of top of qStack.
      int topqStack;
      /// default weight.
      double dw;
      /// Our index.
      const lemur::api::Index &ind;
      /// number of docs in collection (reduce calls to docCount.
      int numDocs;
    };
  }
}

#endif /* _STRUCTQUERYREP_HPP */

