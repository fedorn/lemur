

//
// ExtentEnforcementNode
//
// 31 August 2005 -- ptox
//

#include "indri/ExtentEnforcementNode.hpp"
#include "indri/Annotator.hpp"
#include "lemur-compat.hpp"

indri::infnet::ExtentEnforcementNode::ExtentEnforcementNode( const std::string& name, BeliefNode* child, ListIteratorNode* field ) :
  _name(name),
  _child(child),
  _field(field)
{
}

int indri::infnet::ExtentEnforcementNode::nextCandidateDocument() {
  return _child->nextCandidateDocument();
}

double indri::infnet::ExtentEnforcementNode::maximumBackgroundScore() {
  return INDRI_TINY_SCORE;
}

double indri::infnet::ExtentEnforcementNode::maximumScore() {
  return INDRI_HUGE_SCORE;
}

const indri::utility::greedy_vector<indri::api::ScoredExtentResult>& indri::infnet::ExtentEnforcementNode::score( int documentID, int begin, int end, int documentLength ) {
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
      indri::api::ScoredExtentResult result( childResults[i].score, documentID, childResults[i].begin, childResults[i].end );
      _scores.push_back( result );
    }
  }

  return _scores;
}

void indri::infnet::ExtentEnforcementNode::annotate( indri::infnet::Annotator& annotator, int documentID, int begin, int end ) {
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

bool indri::infnet::ExtentEnforcementNode::hasMatch( int documentID ) {
  return _child->hasMatch( documentID );
}

//
// hasMatch
// 

const indri::utility::greedy_vector<bool>& indri::infnet::ExtentEnforcementNode::hasMatch( int documentID, const indri::utility::greedy_vector<indri::index::Extent>& extents ) {
  // just delegate -- not perfect, but close
  return _child->hasMatch( documentID, extents );
}

//
// getName
//

const std::string& indri::infnet::ExtentEnforcementNode::getName() const {
  return _name;
}

//
// indexChanged
//

void indri::infnet::ExtentEnforcementNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}

