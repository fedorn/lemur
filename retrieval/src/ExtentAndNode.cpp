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
// ExtentAndNode
//
// 6 July 2004 -- tds
//

#include "indri/ExtentAndNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

void ExtentAndNode::_and( greedy_vector<Extent>& out, const greedy_vector<Extent>& one, const greedy_vector<Extent>& two ) {
  greedy_vector<Extent>::const_iterator oneIter = one.begin();
  greedy_vector<Extent>::const_iterator twoIter = two.begin();

  out.clear();

  Extent current;
  current.begin = 0;
  current.end = 0;

  while( oneIter != one.end() && twoIter != two.end() ) {
    Extent intersection;

    // compute the intersection (may be 0 length)
    intersection.begin = lemur_compat::max( oneIter->begin, twoIter->begin );
    intersection.end = lemur_compat::min( oneIter->end, twoIter->end );
    intersection.begin = lemur_compat::min( intersection.begin, intersection.end );

    if( current.end < intersection.begin ) {
      // if last intersection had non-zero length, put it out in the vector
      if( current.begin < current.end )
        out.push_back( current );

      current = intersection;
    } else {
      // this intersection touches the last intersection,
      // so we'll just put them together
      current.end = intersection.end;
    }

    if( oneIter->end == intersection.end ) {
      oneIter++;
    }

    if( twoIter->end == intersection.end ) {
      twoIter++;
    }
  }
  
  if( current.begin != current.end )
    _extents.push_back( current );
}


ExtentAndNode::ExtentAndNode( const std::string& name, std::vector<ListIteratorNode*>& children ) :
  _children(children),
  _name(name)
{
}

void ExtentAndNode::prepare( int documentID ) {
  _extents.clear();

  if( _children.size() == 2 ) {
    _and( _extents, _children[0]->extents(), _children[1]->extents() );
  } else if( _children.size() > 2 ) {
    greedy_vector<Extent> first;
    greedy_vector<Extent> second;
    unsigned int i;

    // this part is a little complex because I'm trying
    // to avoid copying extent vectors too much
    _and( first, _children[0]->extents(), _children[1]->extents() );

    for( i=2; i<_children.size()-2; i+=2 ) {
      _and( second, first, _children[i]->extents() );
      _and( first, second, _children[i+1]->extents() );
    }

    if( i==_children.size()-1 ) {
      _and( _extents, first, _children[i]->extents() ); 
    } else {
      _extents = first;
    }
  }
}

const greedy_vector<Extent>& ExtentAndNode::extents() {
  return _extents;
}

int ExtentAndNode::nextCandidateDocument() {
  assert( _children.size() );
  int candidate = 0;

  for( unsigned int i=0; i<_children.size(); i++ ) {
    candidate = lemur_compat::max( candidate, _children[i]->nextCandidateDocument() );
  }

  return candidate;
}

const std::string& ExtentAndNode::getName() const {
  return _name;
}

void ExtentAndNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
}

