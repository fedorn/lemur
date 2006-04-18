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
// FieldWildcardNode
//
// 18 June 2005 -- pto
//

#include "indri/FieldWildcardNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

indri::infnet::FieldWildcardNode::FieldWildcardNode( const std::string& name ) :
  _name(name),
  _nextDocument(1),
  _index(0),
  _docIterID(0),
  _docIter(0)
{
}

indri::infnet::FieldWildcardNode::~FieldWildcardNode() {
  delete _docIter;
}

void indri::infnet::FieldWildcardNode::prepare( int documentID ) {
  _extents.clear();


  if (documentID <= _index->documentCount()) {

    while (_docIterID < documentID) {
      _docIterID++;
      _docIter->nextEntry();
    }

    indri::index::TermList * termList = _docIter->currentEntry();
    //    const indri::index::TermList * termList = _index->termList(documentID);
    indri::utility::greedy_vector<indri::index::FieldExtent> inExtents = termList->fields();
    indri::utility::greedy_vector<indri::index::FieldExtent>::iterator innerIter = inExtents.begin(); 

    int lastBegin = -1;
    int lastEnd = -1;
    // stuff all fields into the doc
    indri::index::Extent innerExtent;
    while( innerIter != inExtents.end() ) {
      innerExtent.begin = innerIter->begin;
      innerExtent.end = innerIter->end;
      if ( lastBegin != innerExtent.begin || lastEnd != innerExtent.end ) {
        _extents.push_back( innerExtent );
        lastBegin = innerExtent.begin;
        lastEnd = innerExtent.end;
        //      std::cout << "wild " << documentID << " " << innerIter-> number << " " << innerIter->begin << " " << innerIter->end << "\n";
      }
      innerIter++;
    }
    // delete termList;
    
  }
  _nextDocument = documentID + 1;
  if (_nextDocument > _index->documentCount()) {
    _nextDocument = MAX_INT32;
  }
}

const indri::utility::greedy_vector<indri::index::Extent>& indri::infnet::FieldWildcardNode::extents() {
  return _extents;
}

int indri::infnet::FieldWildcardNode::nextCandidateDocument() {
  return _nextDocument;
}

const std::string& indri::infnet::FieldWildcardNode::getName() const {
  return _name;
}

void indri::infnet::FieldWildcardNode::annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent ) {
  annotator.addMatches( _extents, this, documentID, extent );
}

void indri::infnet::FieldWildcardNode::indexChanged( indri::index::Index& index ) { 
  if ( _docIter != 0 ) {
    delete _docIter;
    _docIter = 0;
  }
  _index = & index;
  _nextDocument = 1;
  _docIterID = 1;
  _docIter = index.termListFileIterator();
  _docIter->startIteration();
}
