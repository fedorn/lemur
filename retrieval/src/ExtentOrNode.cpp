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

ExtentOrNode::ExtentOrNode( const std::string& name, std::vector<ListIteratorNode*>& children ) :
  _children(children),
  _name(name)
{
}

void ExtentOrNode::prepare( int documentID ) {
  _extents.clear();
  greedy_vector<Extent> allExtents;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    allExtents.append( _children[i]->extents().begin(), _children[i]->extents().end() );
  }

  // sort all extents in order of beginning
  std::sort( allExtents.begin(), allExtents.end(), Extent::begins_before_less() );
  Extent current;

  if( allExtents.size() ) {
    current.begin = allExtents[0].begin;
    current.end = allExtents[0].end;

    for( unsigned int i=0; i<allExtents.size(); i++ ) {
      if( allExtents[i].begin > current.end ) {
        _extents.push_back( current );
        current = allExtents[i];
      } else {
        current.end = lemur_compat::max( allExtents[i].end, current.end );
      }
    }

    _extents.push_back( current );
  }
}

const greedy_vector<Extent>& ExtentOrNode::extents() {
  return _extents;
}

int ExtentOrNode::nextCandidateDocument() {
  int candidate = INT_MAX;
  
  for( unsigned int i=0; i<_children.size(); i++ ) {
    candidate = lemur_compat::min( _children[i]->nextCandidateDocument(), candidate );
  }

  return candidate;
}

const std::string& ExtentOrNode::getName() const {
  return _name;
}

void ExtentOrNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );

  for( unsigned int i=0; i<_extents.size(); i++ ) {
    for( unsigned int j=0; j<_children.size(); j++ ) {
      _children[j]->annotate( annotator, documentID, _extents[i].begin, _extents[i].end );
    }
  }
}


