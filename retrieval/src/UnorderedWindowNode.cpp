/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
*/


//
// UnorderedWindowNode
//
// 28 July 2004 -- tds
//

#include "indri/UnorderedWindowNode.hpp"
#include "indri/Annotator.hpp"
#include <algorithm>

UnorderedWindowNode::UnorderedWindowNode( const std::string& name, std::vector<ListIteratorNode*>& children ) :
  _name(name),
  _children(children),
  _windowSize(-1) // unlimited window size
{
}

UnorderedWindowNode::UnorderedWindowNode( const std::string& name, std::vector<ListIteratorNode*>& children, int windowSize ) :
  _name(name),
  _children(children),
  _windowSize(windowSize)
{
}

int UnorderedWindowNode::nextCandidateDocument() {
  int maxDocument = 0;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    int current = _children[i]->nextCandidateDocument();
    if( current > maxDocument )
      maxDocument = current;
  }

  return maxDocument;
}

//
// Let each term occurrence be a (type, position) pair (where type is the
// type of term.  Then, we're trying to return every possible subset of 
// (type, position) pairs where we have exactly one of each type.  This is
// a computationally intensive job.
//
// However, note that some returned sets will strictly dominate others.  
// Specifically, when these sets get to a ListBeliefNode, extents that
// overlap will get thrown out.  Therefore, if there is some set
// that spans positions [x,y], and another set that spans [a,b], where
// x = a and y < b, [x,y] strictly dominates [a,b].  Anywhere that 
// [a,b] would be a match, [x,y] will also be a match (and will overlap with
// [a,b]), so we can throw out [a,b].
//
// This suggests an algorithm for finding unordered windows.  We take each
// term position pair in turn, and find the smallest window that includes it
// as the first term.  That covers all possibilities.
//

void UnorderedWindowNode::prepare( int documentID ) {
  _extents.clear();
  assert( _children.size() >= 2 );
  greedy_vector<term_position> allPositions;
  int termsSeen = 0;

  // add every term position from every list
  for( unsigned int i=0; i<_children.size(); i++ ) {
    const greedy_vector<Extent>& childPositions = _children[i]->extents();

    if( childPositions.size() )
      termsSeen++;

    for( unsigned int j=0; j<childPositions.size(); j++ ) {
      term_position p;

      p.type = i;
      p.begin = childPositions[j].begin;
      p.end = childPositions[j].end;
      p.last = -1;

      allPositions.push_back( p );
    }
  }

  // if not all the terms appear in this document,
  // there's no use looking for a window
  if( termsSeen != _children.size() )
    return;

  // sort all positions by <begin> index
  std::sort( allPositions.begin(), allPositions.end() );
  greedy_vector<int> lastPositions;
  lastPositions.resize(_children.size());
  std::fill( lastPositions.begin(), lastPositions.end(), -1 );

  // determine last pointers
  for( int i=0; i<allPositions.size(); i++ ) {
    allPositions[i].last = lastPositions[allPositions[i].type];
    lastPositions[allPositions[i].type] = i;
  }

  // loop over all term occurrences
  for( int i=0; i<allPositions.size(); i++ ) {
    int termsFound = 1;
    unsigned int current;

    for( current=i+1; current < allPositions.size() && termsFound != _children.size(); current++ ) {
      if( (allPositions[current].end - allPositions[i].begin > _windowSize) && (_windowSize >= 0) )
        break;

      // if the last time this term appeared was before the beginning of this window,
      // then this is a new term for this window
      if( allPositions[current].last < i ) {
        termsFound++;
      } 
    }

    if( termsFound == _children.size() ) {
      _extents.push_back( Extent( allPositions[i].begin, allPositions[current-1].end ) );
    }
  }
}

const greedy_vector<Extent>& UnorderedWindowNode::extents() {
  return _extents;
}

const std::string& UnorderedWindowNode::getName() const {
  return _name;
}

void UnorderedWindowNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );

  for( size_t i=0; i<_extents.size(); i++ ) {
    for( size_t j=0; j<_children.size(); j++ ) {
      _children[j]->annotate( annotator, documentID, _extents[i].begin, _extents[i].end );
    }
  }
}
