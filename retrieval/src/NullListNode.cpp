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
// NullListNode
//
// 11 August 2004 -- tds
//
// Like NullScorerNode, but in a list context
//

#include "indri/NullListNode.hpp"

NullListNode::NullListNode( const std::string& name, bool stopword ) : _name(name), _stopword(stopword)
{
}

bool NullListNode::isStopword() const {
  return _stopword;
}

const std::string& NullListNode::getName() const {
  return _name;
}

int NullListNode::nextCandidateDocument() {
  return MAX_INT32;
}

void NullListNode::prepare( int documentID ) {
  // do nothing
}

const greedy_vector<Extent>& NullListNode::extents() {
  return _extents;
}

void NullListNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  // do nothing
}
