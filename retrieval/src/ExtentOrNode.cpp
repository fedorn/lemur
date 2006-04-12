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
// 26 July 2004 -- tds
//

#include "indri/ExtentOrNode.hpp"
#include <algorithm>
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

indri::infnet::ExtentOrNode::ExtentOrNode( const std::string& name, std::vector<ListIteratorNode*>& children ) :
  _children(children),
  _name(name)
{
}

void indri::infnet::ExtentOrNode::prepare( int documentID ) {
  _extents.clear();
  indri::utility::greedy_vector<indri::index::Extent> allExtents;

  // put all extents in the same bag
  for( unsigned int i=0; i<_children.size(); i++ ) {
    _extents.append( _children[i]->extents().begin(), _children[i]->extents().end() );
  }

  // sort all extents in order of beginning
  std::sort( _extents.begin(), _extents.end(), indri::index::Extent::begins_before_less() );
}

const indri::utility::greedy_vector<indri::index::Extent>& indri::infnet::ExtentOrNode::extents() {
  return _extents;
}

int indri::infnet::ExtentOrNode::nextCandidateDocument() {
  int candidate = INT_MAX;
  
  for( unsigned int i=0; i<_children.size(); i++ ) {
    candidate = lemur_compat::min( _children[i]->nextCandidateDocument(), candidate );
  }

  return candidate;
}

const std::string& indri::infnet::ExtentOrNode::getName() const {
  return _name;
}

void indri::infnet::ExtentOrNode::annotate( class indri::infnet::Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  annotator.addMatches( _extents, this, documentID, extent );

  for( unsigned int i=0; i<_extents.size(); i++ ) {
    for( unsigned int j=0; j<_children.size(); j++ ) {
      _children[j]->annotate( annotator, documentID, (indri::index::Extent &)_extents[i] );
    }
  }
}

void indri::infnet::ExtentOrNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}


