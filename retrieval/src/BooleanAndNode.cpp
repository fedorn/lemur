
//
// BooleanAndNode
//
// 16 November 2004 -- tds
// 

#include "indri/BooleanAndNode.hpp"
#include "indri/Annotator.hpp"

BooleanAndNode::BooleanAndNode( const std::string& name, std::vector<ListIteratorNode*>& children ) :
  _name(name),
  _lists(children)
{
}

void BooleanAndNode::prepare( int documentID ) {
  _extents.clear();

  // check for and condition
  for( int i=0; i<_lists.size(); i++ ) {
    if( _lists[i]->extents().size() == 0 )
      return;
  }

  // if all here, make a null extent
  _extents.push_back( Extent( 0, 1 ) );
}

greedy_vector<Extent>& BooleanAndNode::extents() {
  return _extents;
}

int BooleanAndNode::nextCandidateDocument() {
  int document = _lists[0]->nextCandidateDocument();

  for( int i=1; i<_lists.size(); i++ ) {
    document = lemur_compat::max( document, _lists[i]->nextCandidateDocument() );
  }
    
  return document;
}

const std::string& BooleanAndNode::getName() const {
  return _name;
}

void BooleanAndNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );
}

