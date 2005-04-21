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
// ContextCountAccumulator
//
// 24 February 2004 -- tds
//

#include "indri/ContextCountAccumulator.hpp"
#include <assert.h>
#include <vector>
#include "indri/QuerySpec.hpp"
#include "indri/EvaluatorNode.hpp"
#include "indri/DocumentCount.hpp"

indri::infnet::ContextCountAccumulator::ContextCountAccumulator( const std::string& name, ListIteratorNode* matches, ListIteratorNode* context ) :
  _name(name),
  _matches(matches),
  _context(context),
  _occurrences(0),
  _contextSize(0)
{
}

indri::infnet::ContextCountAccumulator::~ContextCountAccumulator() {
}

const std::string& indri::infnet::ContextCountAccumulator::getName() const {
  return _name;
}

double indri::infnet::ContextCountAccumulator::getOccurrences() const {
  return _occurrences;
}

double indri::infnet::ContextCountAccumulator::getContextSize() const {
  return _contextSize;
}

const indri::infnet::EvaluatorNode::MResults& indri::infnet::ContextCountAccumulator::getResults() {
  // we must be finished, so now is a good time to add our results to the ListCache
  _results.clear();

  _results[ "occurrences" ].push_back( indri::api::ScoredExtentResult( _occurrences, 0 ) );
  _results[ "contextSize" ].push_back( indri::api::ScoredExtentResult( _contextSize, 0 ) );

  return _results;
}

const indri::infnet::ListIteratorNode* indri::infnet::ContextCountAccumulator::getContextNode() const {
  return _context;
}

const indri::infnet::ListIteratorNode* indri::infnet::ContextCountAccumulator::getMatchesNode() const {
  return _matches;
}

void indri::infnet::ContextCountAccumulator::evaluate( int documentID, int documentLength ) {
  double documentOccurrences = 0; 
  double documentContextSize = 0;

   if( !_context ) {
     for( size_t i=0; i<_matches->extents().size(); i++ ) {
       const indri::index::Extent& extent = _matches->extents()[i];
       documentOccurrences += extent.weight;
     }
     _occurrences += documentOccurrences;
   } else {

    const indri::utility::greedy_vector<indri::index::Extent>& matches = _matches->extents();
    const indri::utility::greedy_vector<indri::index::Extent>& extents = _context->extents();
    unsigned int ex = 0;
    
    for( unsigned int i=0; i<matches.size() && ex < extents.size(); i++ ) {
      while( ex < extents.size() && matches[i].begin < extents[ex].begin )
        ex++;

      if( ex < extents.size() &&
        matches[i].begin >= extents[ex].begin &&
        matches[i].end <= extents[ex].end ) {
        documentOccurrences += matches[i].weight;
      }
    }

    for( unsigned int i=0; i<extents.size(); i++ ) {
      documentContextSize += extents[i].end - extents[i].begin;
    }

    _occurrences += documentOccurrences;
    _contextSize += documentContextSize;
   }
}


int indri::infnet::ContextCountAccumulator::nextCandidateDocument() {
  int candidate = _matches->nextCandidateDocument();

  if( _context ) {
    candidate = lemur_compat::min( candidate, _context->nextCandidateDocument() );
  }

  return candidate;
}

//
// indexChanged
//

void indri::infnet::ContextCountAccumulator::indexChanged( indri::index::Index& index ) {
  if( ! _context ) {
    _contextSize += index.termCount();
  }
}




