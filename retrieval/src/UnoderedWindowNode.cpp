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

void UnorderedWindowNode::prepare( int documentID ) {
  _extents.clear();
  assert( _children.size() >= 2 );

  std::vector<positions_pointer> lists;
  lists.resize(_children.size());

  // initialize children indices
  for( unsigned int i=0; i<_children.size(); i++ ) {
    const greedy_vector<Extent>& childPositions = _children[i]->extents();
    lists[i].iter = childPositions.begin();
    lists[i].end = childPositions.end();

    if( lists[i].iter == lists[i].end ) {
      // if one word doesn't appear, then there's no use
      // looking for any occurrences
      return;
    }
  }

  // now, find every position possible
  for( ; lists[0].iter != lists[0].end; (lists[0].iter)++ ) {
    int firstPosition = lists[0].iter->begin;
    int previousPosition = firstPosition;
    bool match = true;

    for( unsigned int i=1; i<lists.size(); i++ ) {
      while( lists[i].iter->end < firstPosition ) {
        lists[i].iter++;
        // if we're out of word positions for any term, then we're done
        if( lists[i].iter == lists[i].end )
          return;
      }

      if( lists[i].iter->end - firstPosition > _windowSize ) {
        // word <i> appears too far from the first word
        match = false;
        break;
      } else {
        previousPosition = lists[i].iter->end;
      }
    }

    assert( !match || (previousPosition - firstPosition <= _windowSize) );

    // TODO: check on this--here we're saying that #uw4( health care )
    //       counts three times in this string "health health health care"
    //       I'm not sure that's right

    if( match ) {
      _extents.push_back( Extent( firstPosition, previousPosition ) );
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
