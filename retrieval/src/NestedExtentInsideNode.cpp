

//
// NestedExtentInsideNode
//
// 30 August 2005 -- pto
//

#include "indri/NestedExtentInsideNode.hpp"
#include "lemur-compat.hpp"
#include "indri/Annotator.hpp"

indri::infnet::NestedExtentInsideNode::NestedExtentInsideNode( const std::string& name, ListIteratorNode* inner, ListIteratorNode* outer ) :
  _inner(inner),
  _outer(outer),
  _name(name)
{
}

void indri::infnet::NestedExtentInsideNode::prepare( int documentID ) {
  _extents.clear();

  if( !_inner || !_outer )
    return;

  const indri::utility::greedy_vector<indri::index::Extent>& inExtents = _inner->extents();
  const indri::utility::greedy_vector<indri::index::Extent>& outExtents = _outer->extents();

  indri::utility::greedy_vector<indri::index::Extent>::const_iterator innerIter = inExtents.begin();
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator outerIter = outExtents.begin();

  // with field wildcard node possibly below, we can only move the
  // outer iterator when we are sure that it doesn't overlap with the inner
  // MAJOR flaw for now: we are assuming the Outer extent list is nice and does not contain
  // overlapping extents, and is in order of increasing begin
  // we also assume that for any later extent in the inner iterator the begin >= current begin
  while( innerIter != inExtents.end() && outerIter != outExtents.end() ) {
    if( outerIter->contains( *innerIter ) ) {
      // Found a match, we can now increment the innerIter
      _extents.push_back( *innerIter );
      innerIter++;
    } else if( outerIter->end < innerIter->begin ) {
      // If the outerIter end is less than the innerIter begin, then it cannot contain
      // the innerIter or later innerExtents      
      outerIter++;
    } else { 
      // Default case, the innerIter extent is not inside outerExtent, and will not be
      // inside later outerExtents.
      innerIter++;
    }
  }
}

const indri::utility::greedy_vector<indri::index::Extent>& indri::infnet::NestedExtentInsideNode::extents() {
  return _extents;
}

int indri::infnet::NestedExtentInsideNode::nextCandidateDocument() {
  return lemur_compat::max( _inner->nextCandidateDocument(), _outer->nextCandidateDocument() );
}

const std::string& indri::infnet::NestedExtentInsideNode::getName() const {
  return _name;
}

void indri::infnet::NestedExtentInsideNode::annotate( class Annotator& annotator, int documentID, int begin, int end ) {
  annotator.addMatches( _extents, this, documentID, begin, end );

  indri::index::Extent range( begin, end );
  indri::utility::greedy_vector<indri::index::Extent>::const_iterator iter;
  iter = std::lower_bound( _extents.begin(), _extents.end(), range, indri::index::Extent::begins_before_less() );
  
  for( unsigned int i = iter-_extents.begin(); i<_extents.size() && _extents[i].begin <= end; i++ ) {
    _inner->annotate( annotator, documentID, _extents[i].begin, _extents[i].end );
    _outer->annotate( annotator, documentID, _extents[i].begin, _extents[i].end );
  }
}

void indri::infnet::NestedExtentInsideNode::indexChanged( indri::index::Index& index ) {
  // do nothing
}


