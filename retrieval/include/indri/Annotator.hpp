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
// Annotator
//
// 19 July 2004 -- tds
//

#ifndef INDRI_ANNOTATOR_HPP
#define INDRI_ANNOTATOR_HPP

#include <map>
#include <vector>
#include "indri/InferenceNetworkNode.hpp"
#include "indri/BeliefNode.hpp"
#include "indri/EvaluatorNode.hpp"
#include "indri/Extent.hpp"

class Annotator : public EvaluatorNode {
private:
  BeliefNode* _belief;
  EvaluatorNode::MResults _annotations;
  std::string _name;

public:
  Annotator( const std::string& name, BeliefNode* belief );
  void add( InferenceNetworkNode* node, int documentID, int begin, int end );
  void addMatches( greedy_vector<Extent>& extents, InferenceNetworkNode* node, int documentID, int begin, int end );
  void evaluate( int documentID, int documentLength );
  int nextCandidateDocument();
  EvaluatorNode::MResults& getResults();
  const std::string& getName() const;
  const EvaluatorNode::MResults& getResults() const;
};

#endif // INDRI_ANNOTATOR_HPP
