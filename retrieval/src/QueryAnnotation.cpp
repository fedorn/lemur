/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
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
