
//
// BooleanAndNode
//
// 16 November 2004 -- tds
//

#ifndef INDRI_BOOLEANANDNODE_HPP
#define INDRI_BOOLEANANDNODE_HPP

#include "indri/ListIteratorNode.hpp"
#include "indri/greedy_vector"
#include "lemur/lemur-platform.h"
#include <vector>
#include <string>

class BooleanAndNode : public ListIteratorNode {
  std::vector<ListIteratorNode*> _lists;
  std::string _name;
  greedy_vector<Extent> _extents;

public:
  BooleanAndNode( const std::string& name, std::vector<ListIteratorNode*>& children );

  void prepare( int documentID );
  greedy_vector<Extent>& extents();
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_BOOLEANANDNODE_HPP

