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
// FieldIteratorNode
//
// 23 February 2004 -- tds
//

#ifndef INDRI_FIELDITERATORNODE_HPP
#define INDRI_FIELDITERATORNODE_HPP

#include "indri/ListIteratorNode.hpp"
#include "indri/FieldListIterator.hpp"

class FieldIteratorNode : public ListIteratorNode {
private:
  class indri::index::FieldListIterator* _field;
  greedy_vector<Extent> _extents;
  greedy_vector<INT64> _numbers;
  std::string _name;

public:
  FieldIteratorNode( const std::string& name, class indri::index::FieldListIterator* field );
  void prepare( int documentID );
  /// returns a list of intervals describing positions of children
  const greedy_vector<Extent>& extents(); 
  const greedy_vector<INT64>& numbers();
  int nextCandidateDocument();
  const std::string& getName() const;
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_FIELDITERATORNODE_HPP

