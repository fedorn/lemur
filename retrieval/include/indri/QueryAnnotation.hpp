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

#ifndef INDRI_QUERYANNOTATION_HPP
#define INDRI_QUERYANNOTATION_HPP

#include "indri/EvaluatorNode.hpp"
#include <string>
#include <vector>
#include <map>
#include "indri/QuerySpec.hpp"

struct QueryAnnotationNode {
  std::string name;
  std::string type;
  std::string queryText;
  std::vector<QueryAnnotationNode*> children;
};

class QueryAnnotation {
private:
  QueryAnnotationNode* _queryTree;
  EvaluatorNode::MResults _annotations;
  std::vector<ScoredExtentResult> _results;

public:
  QueryAnnotation();
  QueryAnnotation( indri::lang::Node* queryRoot, EvaluatorNode::MResults& annotations, std::vector<ScoredExtentResult>& results );
  ~QueryAnnotation();

  const QueryAnnotationNode* getQueryTree() const;
  const std::map< std::string, std::vector<ScoredExtentResult> >& getAnnotations() const;
  const std::vector<ScoredExtentResult>& getResults() const;
};

#endif // INDRI_DOCUMENTANNOTATION_HPP
