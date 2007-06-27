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
// OrderedWindowNode
//
// 28 July 2004 -- tds
//

#include "indri/OrderedWindowNode.hpp"
#include "indri/Annotator.hpp"

indri::infnet::OrderedWindowNode::OrderedWindowNode( const std::string& name, const std::vector<indri::infnet::ListIteratorNode*>& children ) :
  _children(children),
  _windowSize(-1), // unlimited window size
  _name(name)
{
  _pointers.resize(children.size());
}

indri::infnet::OrderedWindowNode::OrderedWindowNode( const std::string& name, const std::vector<indri::infnet::ListIteratorNode*>& children, int windowSize ) :
  _children(children),
  _windowSize(windowSize),
  _name(name)
{
  _pointers.resize(children.size());
}

int indri::infnet::OrderedWindowNode::nextCandidateDocument() {
  int maxDocument = 0;

  for( size_t i=0; i<_children.size(); i++ ) {
    int current = _children[i]->nextCandidateDocument();
    if( current > maxDocument )
      maxDocument = current;
  }

  return maxDocument;
}

void indri::infnet::OrderedWindowNode::prepare( int documentID ) {
  _extents.clear();
  assert( _children.size() >= 2 );

  // TODO: could make this faster:
  //          sort incoming words by the number of times they occur
  //          do initial matching with the infrequent terms
  //          check the candidate matches with the more frequent ones.

  // initialize children indices
  for( size_t i=0; i<_children.size(); i++ ) {
    const indri::utility::greedy_vector<indri::index::Extent>& childPositions = _children[i]->extents();
    _pointers[i].iter = childPositions.begin();
    _pointers[i].end = childPositions.end();

    if( _pointers[i].iter == _pointers[i].end ) {
      // if one word doesn't appear, then there's no use
      // looking for any occurrences
      return;
    }
  }

  // now, find every position possible
  // the outer loop iterates over the first word in the sequence,
  // while the inner loop iterates over the remaining words
  for( ; _pointers[0].iter != _pointers[0].end; (_pointers[0].iter)++ ) {
    bool match = true;
    double weight = 1.0;

    for( size_t i=1; i<_pointers.size(); i++ ) {
      // try to find the first occurrence of this term that might
      // possibly work (has to start after the last one ends; this breaks CMU's #0 property syntax)
      while( _pointers[i].iter->begin < _pointers[i-1].iter->end ) {
        _pointers[i].iter++;
        // if we're out of word positions for any term, then we're done
        if( _pointers[i].iter == _pointers[i].end )
          return;
      }

      // now check the distance.  It's a match if there are fewer
      // than _windowSize-1 positions between the end of the last term
      // and the beginning of this one
      if( (_pointers[i].iter->begin - _pointers[i-1].iter->end + 1 > _windowSize) && (_windowSize >= 0) ) {
        // word <i> appears too far from the last word
        match = false;
        break;
      } else {
        weight *= _pointers[i].iter->weight;
      }
    }

    if( match ) {
      // the match extent spans the beginning of the first term and the end of the
      // last term
      _extents.push_back( indri::index::Extent( weight, _pointers.front().iter->begin, _pointers.back().iter->end ) );
    }
  }
}

const indri::utility::greedy_vector<indri::index::Extent>& indri::infnet::OrderedWindowNode::extents() {
  return _extents;
}

const std::string& indri::infnet::OrderedWindowNode::getName() const {
  return _name;
}

void indri::infnet::OrderedWindowNode::annotate( Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  annotator.addMatches( _extents, this, documentID, extent );
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
  iter = std::lower_bound( _extents.begin(), _extents.end(), extent, indri::index::Extent::begins_before_less() );

  while( iter != _extents.end() ) {
    for( size_t j=0; j<_children.size(); j++ ) {
      indri::index::Extent e = (*iter);
      if (extent.contains(e)) {
        _children[j]->annotate( annotator, documentID, e );
      }
    }
    iter++;
  }
}

void indri::infnet::OrderedWindowNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}
