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
// ExtentInsideNode
//
// 28 July 2004 -- tds
//

#include "indri/ExtentInsideNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

ExtentInsideNode::ExtentInsideNode( const std::string& name, ListIteratorNode* inner, ListIteratorNode* outer ) :
  _inner(inner),
  _outer(outer),
  _name(name)
{
}

void ExtentInsideNode::prepare( int documentID ) {
  _extents.clear();

  if( !_inner || !_outer )
    return;

  const greedy_vector<Extent>& inExtents = _inner->extents();
  const greedy_vector<Extent>& outExtents = _outer->extents();

  greedy_vector<Extent>::const_iterator innerIter = inExtents.begin();
  greedy_vector<Extent>::const_iterator outerIter = outExtents.begin();

  while( innerIter != inExtents.end() && outerIter != outExtents.end() ) {
    if( outerIter->contains( *innerIter ) ) {
      _extents.push_back( *innerIter );
      innerIter++;
    } else if( outerIter->begin <= innerIter->begin ) {
      outerIter++;
    } else { 
      innerIter++;
    }
  }
}

const greedy_vector<Extent>& ExtentInsideNode::extents() {
  return _extents;
}

int ExtentInsideNode::nextCandidateDocument() {
  return lemur_compat::max( _inner->nextCandidateDocument(), _outer->nextCandidateDocument() );
}

const std::string& ExtentInsideNode::getName() const {
  return _name;
}

void ExtentInsideNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
  
  for( unsigned int i=0; i<_extents.size(); i++ ) {
    _inner->annotate( annotator, documentID, _extents[i].begin, _extents[i].end );
    _outer->annotate( annotator, documentID, _extents[i].begin, _extents[i].end );
  }
}
