
//
// NotNode
//
// 26 August 2004 -- tds
//

#include "indri/NotNode.hpp"
#include "indri/Annotator.hpp"

NotNode::NotNode( const std::string& name, BeliefNode* child )
  :
  _name(name),
  _child(child)
{
}

// not node never matches
int NotNode::nextCandidateDocument() {
  return MAX_INT32;
}

double NotNode::maximumBackgroundScore() {
  return 1.0 - _child->maximumScore();
}

double NotNode::maximumScore() {
  return 1.0 - _child->maximumBackgroundScore();
}

const greedy_vector<ScoredExtentResult>& NotNode::score( int documentID, int begin, int end, int documentLength ) {
  _extents.clear();
  const greedy_vector<ScoredExtentResult>& child = _child->score( documentID, begin, end, documentLength );

  for( unsigned int i=0; i<child.size(); i++ ) {
    _extents.push_back( ScoredExtentResult( 1.0 - child[i].score, documentID, begin, end ) );
  }

  return _extents;
}

bool NotNode::hasMatch( int documentID ) {
  return _child->hasMatch( documentID );
}

void NotNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.add( this, documentID, begin, end );
  _child->annotate( annotator, documentID, begin, end );
}

const std::string& NotNode::getName() const {
  return _name;
}
