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
// FieldBetweenNode.hpp
//
// 13 April 2004 -- tds
//

#ifndef INDRI_FIELDBETWEENNODE_HPP
#define INDRI_FIELDBETWEENNODE_HPP

#include "indri/ListIteratorNode.hpp"
#include "indri/greedy_vector"
#include "lemur-platform.h"

class FieldBetweenNode : public ListIteratorNode {
private:
  class FieldIteratorNode* _field;
  greedy_vector<Extent> _extents;
  UINT64 _low;
  UINT64 _high;
  std::string _name;

public:
  FieldBetweenNode( const std::string& name, class FieldIteratorNode* iterator, UINT64 low, UINT64 high );

  void prepare( int documentID );
  greedy_vector<Extent>& extents();
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_FIELDBETWEENNODE_HPP

