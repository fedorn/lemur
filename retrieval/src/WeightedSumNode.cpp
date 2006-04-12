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
// WeightedSumNode
//
// 6 July 2004 -- tds
//

#include "indri/WeightedSumNode.hpp"
#include "lemur-compat.hpp"
#include <math.h>
#include "indri/Annotator.hpp"

indri::infnet::WeightedSumNode::WeightedSumNode( const std::string& name ) : _name(name)
{
}

int indri::infnet::WeightedSumNode::nextCandidateDocument() {
  int candidate = MAX_INT32;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    candidate = lemur_compat::min<int>( _children[i]->nextCandidateDocument(), candidate );
  }

  return candidate;
}

double indri::infnet::WeightedSumNode::maximumScore() {
  double s = 0;

  for( unsigned i=0; i<_children.size(); i++ ) {
    s += _weights[i] * exp( _children[i]->maximumScore() );
  }

  return log(s);
}

double indri::infnet::WeightedSumNode::maximumBackgroundScore() {
  double s = 0;

  for( unsigned i=0; i<_children.size(); i++ ) {
    s += _weights[i] * exp( _children[i]->maximumBackgroundScore() );
  }

  return log(s);
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::WeightedSumNode::score( int documentID, indri::index::Extent &extent, int documentLength ) {
  double s = 0;

  for( unsigned i=0; i<_children.size(); i++ ) {
    const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& childResults = _children[i]->score( documentID, extent, documentLength );

    for( unsigned int j=0; j<childResults.size(); j++ ) {
      s += _weights[i] * exp( childResults[j].score );
    }
  }

  _scores.clear();
  _scores.push_back( indri::api::ScoredExtentResult( log(s), documentID, extent.begin, extent.end) );

  return _scores;
}

void indri::infnet::WeightedSumNode::annotate( indri::infnet::Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  annotator.add(this, documentID, extent);

  for( unsigned i=0; i<_children.size(); i++ ) {
    _children[i]->annotate( annotator, documentID, extent );
  }
}

bool indri::infnet::WeightedSumNode::hasMatch( int documentID ) {
  for( unsigned int i=0; i<_children.size(); i++ ) {
    if( _children[i]->hasMatch( documentID ) )
      return true;
  }

  return false;
}

//
// hasMatch
//

const indri::utility::greedy_vector<bool>& indri::infnet::WeightedSumNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) {
  _matches.clear();
  _matches.resize( extents.size(), false );

  for( unsigned int i=0; i<_children.size(); i++ ) {
    const indri::utility::greedy_vector<bool>& kidMatches = _children[i]->hasMatch( documentID, extents );

    for( unsigned int j=0; j<kidMatches.size(); j++ ) {
      if( kidMatches[j] ) {
        _matches[j] = true;
      }
    }
  }

  return _matches;
}

void indri::infnet::WeightedSumNode::addChild( double weight, BeliefNode* child ) {
  _children.push_back(child);
  _weights.push_back(weight);
}

const std::string& indri::infnet::WeightedSumNode::getName() const {
  return _name;
}

void indri::infnet::WeightedSumNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}
