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


