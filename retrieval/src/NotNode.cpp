
//
// NotNode
//
// 26 August 2004 -- tds
//

#include "indri/NotNode.hpp"
#include "indri/Annotator.hpp"
#include <cmath>

NotNode::NotNode( const std::string& name, BeliefNode* child )
  :
  _name(name),
  _child(child)
{
}

// for convenience, the not node never actually matches
int NotNode::nextCandidateDocument() {
  return MAX_INT32;
}

double NotNode::maximumBackgroundScore() {
  // we just want an upper bound on the background score
  // this is a poor upper bound, but it should be accurate
  return log( 1.0 - exp( _child->maximumBackgroundScore() ) );
}

double NotNode::maximumScore() {
  // without a true minimum score available, we don't have a 
  // better estimate for the max than probability = 1.0; 
  // and log(1) = 0.
  return 0.0;
}

const greedy_vector<ScoredExtentResult>& NotNode::score( int documentID, int begin, int end, int documentLength ) {
  _extents.clear();
  const greedy_vector<ScoredExtentResult>& child = _child->score( documentID, begin, end, documentLength );

  for( unsigned int i=0; i<child.size(); i++ ) {
    _extents.push_back( ScoredExtentResult( log( 1.0 - exp(child[i].score) ), documentID, begin, end ) );
  }

  return _extents;
}

bool NotNode::hasMatch( int documentID ) {
  return !_child->hasMatch( documentID );
}

void NotNode::annotate( Annotator& annotator, int documentID, int begin, int end ) {
  annotator.add( this, documentID, begin, end );
  _child->annotate( annotator, documentID, begin, end );
}

const std::string& NotNode::getName() const {
  return _name;
}

