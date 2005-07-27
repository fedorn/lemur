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
// ExtentRestrictionNode
//
// 6 July 2004 -- tds
//

#include "indri/ExtentRestrictionNode.hpp"
#include "indri/Annotator.hpp"
#include "lemur-compat.hpp"

indri::infnet::ExtentRestrictionNode::ExtentRestrictionNode( const std::string& name, BeliefNode* child, ListIteratorNode* field ) :
  _name(name),
  _child(child),
  _field(field)
{
}

int indri::infnet::ExtentRestrictionNode::nextCandidateDocument() {
  return _child->nextCandidateDocument();
}

double indri::infnet::ExtentRestrictionNode::maximumBackgroundScore() {
  return INDRI_TINY_SCORE;
}

double indri::infnet::ExtentRestrictionNode::maximumScore() {
  return INDRI_HUGE_SCORE;
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::ExtentRestrictionNode::score( int documentID, int begin, int end, int documentLength ) {
  // we're going to run through the field list, etc.
  const indri::utility::greedy_vector<bool>& matches = _child->hasMatch( documentID, _field->extents() );
  assert( matches.size() == _field->extents().size() );

  indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
  _scores.clear();


  for( size_t i = 0; i < matches.size(); i++ ) {
    if( !matches[i] )
      continue;

    iter = &_field->extents()[i];

    if( iter->end < begin )
      continue; // this one isn't relevant to our cause

    if( end < iter->begin )
      break; // we've passed all the relevant fields

    if( iter->end - iter->begin == 0 )
      continue; // this field has no text in it

    int scoreBegin = lemur_compat::max( iter->begin, begin );
    int scoreEnd = lemur_compat::min( iter->end, end );

    const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& childResults = _child->score( documentID, scoreBegin, scoreEnd, documentLength );

    for( int i=0; i<childResults.size(); i++ ) {
      indri::api::ScoredExtentResult result( childResults[i].score, documentID, scoreBegin, scoreEnd );
      _scores.push_back( result );
    }
  }

  return _scores;
}

void indri::infnet::ExtentRestrictionNode::annotate( indri::infnet::Annotator& annotator, int documentID, int begin, int end ) {
  const indri::utility::greedy_vector<bool>& matches = _child->hasMatch( documentID, _field->extents() );
  assert( matches.size() == _field->extents().size() );

  annotator.add(this, documentID, begin, end);
  // we're going to run through the field list, etc.

  const indri::utility::greedy_vector<indri::index::Extent>& extents = _field->extents(); 
  int fieldCount = extents.size();
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;

  for( int i=0; i<fieldCount; i++ ) {
    if( !matches[i] )
      continue;

    iter = &extents[i];
  
    if( iter->end < begin )
      continue; // this one isn't relevant to our cause

    if( end < iter->begin )
      break; // we've passed all the relevant fields

    if( iter->end - iter->begin == 0 )
      continue; // this field has no text in it

    int scoreBegin = lemur_compat::max( iter->begin, begin );
    int scoreEnd = lemur_compat::min( iter->end, end );

    _child->annotate( annotator, documentID, scoreBegin, scoreEnd );
  }
}

//
// hasMatch
//

bool indri::infnet::ExtentRestrictionNode::hasMatch( int documentID ) {
  return _child->hasMatch( documentID );
}

//
// hasMatch
// 

const indri::utility::greedy_vector<bool>& indri::infnet::ExtentRestrictionNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) {
  // just delegate -- not perfect, but close
  return _child->hasMatch( documentID, extents );
}

//
// getName
//

const std::string& indri::infnet::ExtentRestrictionNode::getName() const {
  return _name;
}

//
// indexChanged
//

void indri::infnet::ExtentRestrictionNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}

