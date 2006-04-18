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
  _field(field),
 _matches()
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

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::ExtentRestrictionNode::score( int documentID, indri::index::Extent &extent, int documentLength ) {
  // we're going to run through the field list, etc.
  if ( _field != NULL ) {

    indri::index::Extent tExtent(extent.begin, extent.end);
    const indri::utility::greedy_vector<indri::index::Extent>& fieldExtentsTmp = _field->matches( tExtent );
    indri::utility::greedy_vector<indri::index::Extent> fieldExtents;
    fieldExtents.append(fieldExtentsTmp.begin(), fieldExtentsTmp.end());
    
    //   const indri::utility::greedy_vector<bool>& matches = _child->hasMatch( documentID, fieldExtents );
    //   assert( matches.size() == fieldExtents.size() );
    
    indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
    _scores.clear();
    
    for( size_t i = 0; i < fieldExtents.size(); i++ ) {    
      //     if( !matches[i] )  // We actually want to score all, whether or not they have a query term 
      //       continue;        // match.  This will give us proper scores when ther eis not a match.
      
      iter = &(fieldExtents[i]);
      
      if( iter->end - iter->begin == 0 )
        continue; // this field has no text in it
      
      
      int scoreBegin = iter->begin;
      int scoreEnd = iter->end;

//       std::cout << getName() << " " << documentID << " " << begin <<":"<<end << " " << iter->begin <<":"<< iter->end << std::endl;
      
      const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& childResults = _child->score( documentID, (indri::index::Extent&)(*iter), documentLength );
      
      double fieldWeight = iter->weight;
      
      for( int i=0; i<childResults.size(); i++ ) {
        indri::api::ScoredExtentResult result( fieldWeight*childResults[i].score, documentID, scoreBegin, scoreEnd );
        _scores.push_back( result );
      }
    } 

    // do a bad guess if there's no matching field
//     if ( _scores.size() == 0 ) {
//       const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& childResults = _child->score( documentID, extent.begin, extent.begin, documentLength );
//       for( int i=0; i<childResults.size(); i++ ) {
//      indri::api::ScoredExtentResult result( childResults[i].score, documentID, extent.begin, extent.begin );
//      _scores.push_back( result );
//       }
//     }
  } else {
    return _child->score( documentID, extent, documentLength );    
  }

  return _scores;
}

void indri::infnet::ExtentRestrictionNode::annotate( indri::infnet::Annotator& annotator, int documentID, indri::index::Extent &extent ) {

  annotator.add(this, documentID, extent);
  // we're going to run through the field list, etc.

  if ( _field != NULL) {

    indri::index::Extent tExtent(extent.begin, extent.end);
    const indri::utility::greedy_vector<indri::index::Extent>& fieldExtentsTmp = _field->matches( tExtent );
    indri::utility::greedy_vector<indri::index::Extent> fieldExtents;
    fieldExtents.append(fieldExtentsTmp.begin(), fieldExtentsTmp.end());
    
    //   const indri::utility::greedy_vector<bool>& matches = _child->hasMatch( documentID, fieldExtents );
    //   assert( matches.size() == fieldExtents.size() );
    
    indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
    _scores.clear();
    
    for( size_t i = 0; i < fieldExtents.size(); i++ ) {    
      //     if( !matches[i] )  // We actually want to score all, whether or not they have a query term 
      //       continue;        // match.  This will give us proper scores when ther eis not a match.
      
      iter = &(fieldExtents[i]);
      
      if( iter->end - iter->begin == 0 )
        continue; // this field has no text in it
      
      
      int scoreBegin = iter->begin;
      int scoreEnd = iter->end;

      
      _child->annotate( annotator, documentID, (indri::index::Extent&)(*iter));
      
    } 

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
   _matches.clear();
  _matches.resize( extents.size(), false );

  if ( _field != NULL ) {

    for ( size_t i = 0; i < extents.size(); i++ ) {
      const indri::utility::greedy_vector<indri::index::Extent>& fieldExtents = _field->matches( (indri::index::Extent &)extents[i] );
      
      const indri::utility::greedy_vector<bool>& childMatches = _child->hasMatch( documentID, fieldExtents );
      bool match = false;
      for ( size_t j = 0; !match && j < childMatches.size(); j++ ) {
        
      if ( childMatches[ j ] ) {
        match = true;
      }
      }
      _matches[i] = match;
    }
  } else {
    return _child->hasMatch( documentID, extents );
  }
  

  return _matches;
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

