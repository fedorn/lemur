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
// ListIteratorNode
//
// 26 January 2004 -- tds
//

#ifndef INDRI_LISTITERATORNODE_HPP
#define INDRI_LISTITERATORNODE_HPP

#include "indri/InferenceNetworkNode.hpp"
#include "indri/Extent.hpp"
#include <indri/greedy_vector>
namespace indri
{
  namespace infnet 
  {
    class ListIteratorNode : public InferenceNetworkNode {
    public:
      /// sets up as much as we can with just the document ID
      virtual void prepare( int documentID ) = 0;

      /// returns a list of intervals describing positions of children
      virtual const indri::utility::greedy_vector<indri::index::Extent>& extents() = 0;

      /// annotate any results from this node from position begin to position end
      virtual void annotate( class Annotator& annotator, int documentID, int begin, int end ) = 0;
    };
  }
}

#endif // INDRI_LISTNODE_HPP

