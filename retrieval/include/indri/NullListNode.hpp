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
// NullListNode.hpp
//
// 11 August 2004 -- tds
//

#ifndef INDRI_NULLLISTNODE_HPP
#define INDRI_NULLLISTNODE_HPP

#include "indri/ListIteratorNode.hpp"

class NullListNode : public ListIteratorNode {
private:
  greedy_vector<Extent> _extents;
  std::string _name;
  bool _stopword;

public:
  NullListNode( const std::string& name, bool stopword );

  bool isStopword() const;
  const std::string& getName() const;
  int nextCandidateDocument();
  void prepare( int documentID );
  const greedy_vector<Extent>& extents();
  void annotate( class Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_NULLLISTNODE_HPP

