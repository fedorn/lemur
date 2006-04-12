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
// ExtentOrNode
//
// 8 April 2005 -- tds
//

#include "indri/WeightedExtentOrNode.hpp"
#include <algorithm>
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

indri::infnet::WeightedExtentOrNode::WeightedExtentOrNode( const std::string& name, std::vector<ListIteratorNode*>& children, const std::vector<double>& weights ) :
  _weights(weights),  
  _children(children),
  _name(name)
{
}

void indri::infnet::WeightedExtentOrNode::prepare( int documentID ) {
  _extents.clear();
  indri::utility::greedy_vector<indri::index::Extent> allExtents;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    ListIteratorNode* child = _children[i];
    double weight = _weights[i];
    
    for( unsigned int j=0; j<child->extents().size(); j++ ) {
      const indri::index::Extent& extent = child->extents()[j];
      _extents.push_back( indri::index::Extent( weight * extent.weight, extent.begin, extent.end ) );
    }
  }

  // sort all extents in order of beginning
  std::sort( _extents.begin(), _extents.end(), indri::index::Extent::begins_before_less() );
}

const indri::utility::greedy_vector<indri::index::Extent>& indri::infnet::WeightedExtentOrNode::extents() {
  return _extents;
}

int indri::infnet::WeightedExtentOrNode::nextCandidateDocument() {
  int candidate = INT_MAX;
  
  for( unsigned int i=0; i<_children.size(); i++ ) {
    candidate = lemur_compat::min( _children[i]->nextCandidateDocument(), candidate );
  }

  return candidate;
}

const std::string& indri::infnet::WeightedExtentOrNode::getName() const {
  return _name;
}

void indri::infnet::WeightedExtentOrNode::annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  annotator.addMatches( _extents, this, documentID, extent );

  for( unsigned int i=0; i<_extents.size(); i++ ) {
    for( unsigned int j=0; j<_children.size(); j++ ) {
      _children[j]->annotate( annotator, documentID, _extents[i] );
    }
  }
}

void indri::infnet::WeightedExtentOrNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}


