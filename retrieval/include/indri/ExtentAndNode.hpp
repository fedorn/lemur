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
// ExtentAndNode
//
// 23 February 2004 -- tds
//

#ifndef INDRI_EXTENTANDNODE_HPP
#define INDRI_EXTENTANDNODE_HPP

#include "indri/ListIteratorNode.hpp"
#include <vector>
#include "indri/greedy_vector"

class ExtentAndNode : public ListIteratorNode {
private:
  std::vector<ListIteratorNode*> _children;
  greedy_vector<Extent> _extents;
  greedy_vector<Extent> _partialExtents;
  std::string _name;

  void _and( greedy_vector<Extent>& out, const greedy_vector<Extent>& one, const greedy_vector<Extent>& two );

public:
  ExtentAndNode( const std::string& name, std::vector<ListIteratorNode*>& children );
  void prepare( int documentID );
  const greedy_vector<Extent>& extents();
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif //INDRI_EXTENTANDNODE_HPP

