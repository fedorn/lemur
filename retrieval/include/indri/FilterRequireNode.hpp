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
// FilterRequireNode
//
// 13 April 2004 -- tds
//

#ifndef INDRI_FILTERREQUIRENODE_HPP
#define INDRI_FILTERREQUIRENODE_HPP

#include "indri/ListIteratorNode.hpp"
#include "indri/Extent.hpp"

class FilterRequireNode : public ListIteratorNode {
private:
  ListIteratorNode* _filtered;
  ListIteratorNode* _required;
  greedy_vector<Extent> _extents;
  std::string _name;

public:
  FilterRequireNode( const std::string& name, ListIteratorNode* filtered, ListIteratorNode* required );

  void prepare( int documentID );
  const greedy_vector<Extent>& extents();
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_FILTERREQUIRENODE_HPP
