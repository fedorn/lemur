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
// UnorderedWindowNode
//
// 26 January 2004 -- tds
//

#ifndef INDRI_UNORDEREDWINDOWNODE_HPP
#define INDRI_UNORDEREDWINDOWNODE_HPP

#include "indri/ListIteratorNode.hpp"
#include <vector>
#include <indri/greedy_vector>

class UnorderedWindowNode : public ListIteratorNode {
private:
  struct term_position {
    bool operator< ( const term_position& other ) const {
      return begin < other.begin;
    }

    int type;
    int begin;
    int end;
    int last; // index of previous entry of this type
  };

  int _windowSize;
  std::vector<ListIteratorNode*> _children;
  greedy_vector<Extent> _extents;
  std::string _name;

public:
  UnorderedWindowNode( const std::string& name, std::vector<ListIteratorNode*>& children );
  UnorderedWindowNode( const std::string& name, std::vector<ListIteratorNode*>& children, int windowSize );
  int nextCandidateDocument();
  void prepare( int documentID );
  const greedy_vector<Extent>& extents();
  const std::string& getName() const;
  void annotate( Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_UNORDEREDWINDOWNODE_HPP
