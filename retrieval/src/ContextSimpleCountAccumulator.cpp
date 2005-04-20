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
// ContextSimpleCountAccumulator
//
// 14 December 2004 -- tds
//
// Unlike the ContextCountAccumulator, which counts occurrences of
// terms in a very general way, this node uses knowledge about the 
// query tree to extract counts directly from the index.
//
// This node is placed into the query tree by the
// ContextSimpleCountCollectorCopier.
//

#include "indri/ContextSimpleCountAccumulator.hpp"
#include "indri/ScoredExtentResult.hpp"

void indri::infnet::ContextSimpleCountAccumulator::_computeCounts( indri::index::Index& index ) {
  assert( _terms.size() );
  assert( _context.size() == 0 || _field.size() == 0 );

  if( _context.size() ) {
    _size += index.fieldTermCount( _context );
  } else {
    _size += index.termCount();
  }

  for( unsigned int i=0; i<_terms.size(); i++ ) {
    if( _terms[i].length() != 0 ) {
      if( _field.size() ) {
        _occurrences += index.fieldTermCount( _field, _terms[i] );
      } else if( _context.size() ) {
        _occurrences += index.fieldTermCount( _context, _terms[i] );
      } else {
        _occurrences += index.termCount( _terms[i] );
      }
    }
  }
}

indri::infnet::ContextSimpleCountAccumulator::ContextSimpleCountAccumulator( const std::string& nodeName,
                                                              const std::vector<std::string>& terms,
                                                              const std::string& field,
                                                              const std::string& context ) :
  _name(nodeName),
  _terms(terms),
  _field(field),
  _context(context),
  _occurrences(0),
  _size(0)
{
}

const std::string& indri::infnet::ContextSimpleCountAccumulator::getName() const {
  return _name;
}

const indri::infnet::EvaluatorNode::MResults& indri::infnet::ContextSimpleCountAccumulator::getResults() {
  _results.clear();

  _results[ "occurrences" ].push_back( indri::api::ScoredExtentResult( _occurrences, 0 ) );
  _results[ "contextSize" ].push_back( indri::api::ScoredExtentResult( _size, 0 ) );

  return _results;
}

void indri::infnet::ContextSimpleCountAccumulator::indexChanged( indri::index::Index& index ) {
  _computeCounts( index );
  _maximumDocument = index.documentCount() + index.documentBase();
}

void indri::infnet::ContextSimpleCountAccumulator::evaluate( int documentID, int documentLength ) {
  // do nothing
}

int indri::infnet::ContextSimpleCountAccumulator::nextCandidateDocument() {
  return MAX_INT32;
}
