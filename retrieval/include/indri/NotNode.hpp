
//
// NotNode
//
// 26 August 2004 -- tds
//

#ifndef INDRI_NOTNODE_HPP
#define INDRI_NOTNODE_HPP

#include "indri/BeliefNode.hpp"
#include "indri/greedy_vector"
#include <string>

class NotNode : public BeliefNode {
private:
  BeliefNode* _child;
  greedy_vector<ScoredExtentResult> _extents;
  std::string _name;

public:
  NotNode( const std::string& name, BeliefNode* child );

  double maximumBackgroundScore();
  double maximumScore();
  const greedy_vector<ScoredExtentResult>& score( int documentID, int start, int end, int documentLength );
  bool hasMatch( int documentID );
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
  
  int nextCandidateDocument();
  const std::string& getName() const; 
};

#endif // INDRI_NOTNODE_HPP
