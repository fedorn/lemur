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

OrderedWindowNode::OrderedWindowNode( const std::string& name, const std::vector<ListIteratorNode*>& children ) :
  _children(children),
  _windowSize(-1), // unlimited window size
  _name(name)
{
  _pointers.resize(children.size());
}

OrderedWindowNode::OrderedWindowNode( const std::string& name, const std::vector<ListIteratorNode*>& children, int windowSize ) :
  _children(children),
  _windowSize(windowSize),
  _name(name)
{
  _pointers.resize(children.size());
}

int OrderedWindowNode::nextCandidateDocument() {
  int maxDocument = 0;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    int current = _children[i]->nextCandidateDocument();
    if( current > maxDocument )
      maxDocument = current;
  }

  return maxDocument;
}

void OrderedWindowNode::prepare( int documentID ) {
  _extents.clear();
  assert( _children.size() >= 2 );

  // TODO: could make this faster:
  //          sort incoming words by the number of times they occur
  //          do initial matching with the infrequent terms
  //          check the candidate matches with the more frequent ones.

  // initialize children indices
  for( unsigned int i=0; i<_children.size(); i++ ) {
    const greedy_vector<Extent>& childPositions = _children[i]->extents();
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

    for( unsigned int i=1; i<_pointers.size(); i++ ) {
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
      }
    }

    if( match ) {
      // the match extent spans the beginning of the first term and the end of the
      // last term
      _extents.push_back( Extent( _pointers.front().iter->begin, _pointers.back().iter->end ) );
    }
  }
}

const greedy_vector<Extent>& OrderedWindowNode::extents() {
  return _extents;
}

const std::string& OrderedWindowNode::getName() const {
  return _name;
}

void OrderedWindowNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );

  for( size_t i=0; i<_extents.size(); i++ ) {
    for( size_t j=0; j<_children.size(); j++ ) {
      _children[j]->annotate( annotator, documentID, _extents[i].begin, _extents[i].end );
    }
  }
}
