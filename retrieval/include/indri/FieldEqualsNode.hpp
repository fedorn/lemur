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
// FieldEqualsNode.hpp
//
// 26 October 2004 -- tds
//

#ifndef INDRI_FIELDEQUALSNODE_HPP
#define INDRI_FIELDEQUALSNODE_HPP

#include "indri/ListIteratorNode.hpp"
#include "indri/greedy_vector"
#include "lemur-platform.h"

class FieldEqualsNode : public ListIteratorNode {
private:
  class FieldIteratorNode* _field;
  greedy_vector<Extent> _extents;
  INT64 _constant;
  std::string _name;

public:
  FieldEqualsNode( const std::string& name, class FieldIteratorNode* iterator, INT64 constant );

  void prepare( int documentID );
  greedy_vector<Extent>& extents();
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_FIELDEQUALSNODE_HPP

