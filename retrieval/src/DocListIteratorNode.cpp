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
// DocListIteratorNode
//
// 28 July 2004 -- tds
//

#include "indri/DocListIteratorNode.hpp"
#include "indri/Annotator.hpp"

DocListIteratorNode::DocListIteratorNode( const std::string& name, DocPositionInfoList* list ) :
  _list(list),
  _name(name)
{
  _list->startIteration();
  _list->nextEntry();
}

int DocListIteratorNode::nextCandidateDocument() {
  DocInfo* info = _list->currentEntry();

  if( info )
    return info->docID();
  else
    return MAX_INT32;
}

void DocListIteratorNode::prepare( int documentID ) {
  DocInfo* info = _list->currentEntry();
  _extents.clear();

  if( !info || info->docID() != documentID )
    return;

  int count = info->termCount();
  // dmf FIXME
  const int* pos = (const int *) info->positions();
  
  for( int i = 0; i < count; i++ ) {
    _extents.push_back( Extent( pos[i], pos[i]+1 ) );
  }
}

const greedy_vector<Extent>& DocListIteratorNode::extents() {
  return _extents;
}

const std::string& DocListIteratorNode::getName() const {
  return _name;
}

void DocListIteratorNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
}
