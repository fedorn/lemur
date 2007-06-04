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
// ListAccumulator
//
// 11 April 2006 -- tds
//
// This class exists only for the QueryEnvironment::expressionList
// call.  Its job is to collect the occurrences of a particular 
// query language expression, and to return those occurrences to
// the caller.
//

#include "indri/ListAccumulator.hpp"
#include "indri/ScoredExtentResult.hpp"

indri::infnet::ListAccumulator::ListAccumulator( const std::string& nodeName,
                                                 indri::infnet::ListIteratorNode& iterNode ) :
  _name(nodeName),
  _counted(iterNode)
{
  _results[ "occurrences" ].clear();
  _resultVector = &_results[ "occurrences" ];
}

indri::infnet::ListAccumulator::~ListAccumulator() {
}

const std::string& indri::infnet::ListAccumulator::getName() const {
  return _name;
}

const indri::infnet::EvaluatorNode::MResults& indri::infnet::ListAccumulator::getResults() {
  return _results;
}

void indri::infnet::ListAccumulator::indexChanged( indri::index::Index& index ) {
}

void indri::infnet::ListAccumulator::evaluate( int documentID, int documentLength ) {
  const indri::utility::greedy_vector<indri::index::Extent>& extents = _counted.extents();
  
  for( size_t i=0; i<extents.size(); i++ ) {
    _resultVector->push_back( indri::api::ScoredExtentResult( extents[i].weight, documentID, extents[i].begin, extents[i].end ) );
  }
}

int indri::infnet::ListAccumulator::nextCandidateDocument() {
  return _counted.nextCandidateDocument();
}

