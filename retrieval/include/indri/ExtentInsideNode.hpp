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
// ExtentInsideNode
//
// 23 February 2004 -- tds
//
// The difference between an include node and an
// and node is that in the include node, the inner
// extent must be contained completely within an
// outer extent.
//

#ifndef INDRI_EXTENTINSIDENODE_HPP
#define INDRI_EXTENTINSIDENODE_HPP

#include "indri/ListIteratorNode.hpp"
#include "indri/greedy_vector"
#include "indri/Extent.hpp"

class ExtentInsideNode : public ListIteratorNode {
  ListIteratorNode* _inner;
  ListIteratorNode* _outer;
  greedy_vector<Extent> _extents;
  std::string _name;

public:
  ExtentInsideNode( const std::string& name, ListIteratorNode* inner, ListIteratorNode* outer );

  void prepare( int documentID );
  const greedy_vector<Extent>& extents();
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_EXTENTISIDENODE_HPP

