/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
*/


//
// ContextCountGraphCopier
//
// 5 March 2004 -- tds
//

#ifndef INDRI_CONTEXTCOUNTGRAPHCOPIER_HPP
#define INDRI_CONTEXTCOUNTGRAPHCOPIER_HPP

class ContextCountGraphCopier : public indri::lang::Copier {
private:
  std::vector<indri::lang::Node*> _newNodes;
  std::vector<indri::lang::ContextCounterNode*> _contextCounters;

public:
  ~ContextCountGraphCopier() {
    delete_vector_contents<indri::lang::Node*>( _newNodes );
  }

  indri::lang::Node* defaultAfter( indri::lang::Node* oldNode, indri::lang::Node* newNode ) {
    _newNodes.push_back( newNode );
    return newNode;
  }

  indri::lang::Node* after( indri::lang::RawScorerNode* oldNode, indri::lang::RawScorerNode* newNode ) {
    indri::lang::RawExtentNode* raw = newNode->getRawExtent();
    indri::lang::RawExtentNode* context = newNode->getContext();
    delete newNode;

    indri::lang::ContextCounterNode* contextCounter = new indri::lang::ContextCounterNode( raw, context );
    // need to match the name of the raw scorer we're counting for
    contextCounter->setNodeName( oldNode->nodeName() );
    _newNodes.push_back( contextCounter );
    _contextCounters.push_back( contextCounter );
    return contextCounter;
  }

  std::vector<indri::lang::Node*>& getNodes() {
    return _newNodes;
  }

  std::vector<indri::lang::ContextCounterNode*>& getContextCounterNodes() {
    return _contextCounters;
  }
};

#endif // INDRI_CONTEXTCOUNTGRAPHCOPIER_HPP

