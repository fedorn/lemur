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
// QueryAnnotation
// 
// 21 July 2004 -- tds
//

#include "indri/QueryAnnotation.hpp"
#include "indri/Walker.hpp"
#include <map>
#include <set>
#include <stack>
#include "indri/QuerySpec.hpp"

class QueryNodeBuilder : public indri::lang::Walker {
private:
  QueryAnnotationNode* _root;
  std::map< indri::lang::Node*, QueryAnnotationNode* > _nodes;
  std::stack< QueryAnnotationNode* > _stack;

public:
  QueryNodeBuilder() : _root(0) {}

  void defaultBefore( indri::lang::Node* n ) {
    std::map< indri::lang::Node*, QueryAnnotationNode* >::iterator iter = _nodes.find(n);
    QueryAnnotationNode* next = 0;

    if( iter != _nodes.end() ) {
      next = iter->second;
    } else {
      next = new QueryAnnotationNode;
      next->name = n->nodeName();
      next->queryText = n->queryText();
      next->type = n->typeName();
    }

    if( _stack.size() ) {
      _stack.top()->children.push_back( next );
    }

    _stack.push( next );

    if( !_root ) {
      _root = next;
    }
  }

  void defaultAfter( indri::lang::Node* n ) {
    _stack.pop();
  }

  QueryAnnotationNode* getRoot() {
    return _root;
  }
};

void delete_query_node( QueryAnnotationNode* node, std::set<QueryAnnotationNode*>& deleted ) {
  // query tree may be a dag, so we have to be careful
  deleted.insert(node);
  
  for( unsigned int i=0; i<node->children.size(); i++ ) {
    QueryAnnotationNode* child = node->children[i];
    if( deleted.find(child) != deleted.end() )
      delete_query_node(node->children[i], deleted);
  }
}


QueryAnnotation::QueryAnnotation() :
  _queryTree(0)
{
}

QueryAnnotation::~QueryAnnotation() {
  std::set<QueryAnnotationNode*> deleted;
  delete_query_node(_queryTree, deleted);
}

QueryAnnotation::QueryAnnotation( indri::lang::Node* queryRoot, EvaluatorNode::MResults& annotations, std::vector<ScoredExtentResult>& results ) {
  QueryNodeBuilder builder;
  queryRoot->walk(builder);
  _queryTree = builder.getRoot();
  _annotations = annotations;
  _results = results;
}

const QueryAnnotationNode* QueryAnnotation::getQueryTree() const {
  return _queryTree;
}

const std::vector<ScoredExtentResult>& QueryAnnotation::getResults() const {
  return _results;
}

const EvaluatorNode::MResults& QueryAnnotation::getAnnotations() const {
  return _annotations;
}
