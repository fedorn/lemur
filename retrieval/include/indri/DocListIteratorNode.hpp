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
// DocListIteratorNode
//
// 26 January 2004 -- tds
//

#ifndef INDRI_DOCLISTITERATORNODE_HPP
#define INDRI_DOCLISTITERATORNODE_HPP

#include "indri/DocPositionInfoList.hpp"
#include "indri/ListIteratorNode.hpp"

class DocListIteratorNode : public ListIteratorNode {
private:
  DocPositionInfoList* _list;
  greedy_vector<Extent> _extents;
  std::string _name;

public:
  DocListIteratorNode( const std::string& name, DocPositionInfoList* list );

  int nextCandidateDocument();
  void prepare( int documentID );
  const greedy_vector<Extent>& extents();
  const std::string& getName() const;
  void annotate( Annotator& annotator, int documentID, int begin, int end );
};

#endif // INDRI_DOCLISTITERATORNODE_HPP
