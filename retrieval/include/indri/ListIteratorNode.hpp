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
    protected:
      indri::utility::greedy_vector<indri::index::Extent> _matches;
     
    public:
      /// sets up as much as we can with just the document ID
      virtual void prepare( int documentID ) = 0;

      /// returns a list of intervals describing positions of children
      virtual const indri::utility::greedy_vector<indri::index::Extent>& extents() = 0;

      /// annotate any results from this node from position begin to position end
      virtual void annotate( class Annotator& annotator, int documentID, indri::index::Extent &extent ) = 0;

      // Moved work of identifying matches of an extent from ExtentRestriction to here.
      // This allows the computation for the list of matches for an extent to be 
      // overridden.  ExtentParent, ExtentChild, and ExtentDescendant are among the
      // classes that need to do this, as the parent/child/descedant relationships
      // are not based on extent containment - these relationships may be among
      // arbitrary fields in a document.
      virtual const indri::utility::greedy_vector<indri::index::Extent>& matches( indri::index::Extent &extent ) {
        int begin = extent.begin;
        int end = extent.end;
        const indri::utility::greedy_vector<indri::index::Extent>& exts = extents();
        _matches.clear();
        for( size_t i = 0 ; i < exts.size(); i++ ) {
          if ( begin <= exts[i].begin && end >= exts[i].end ) {
            _matches.push_back( exts[i] );
          } else if ( exts[i].begin > end ) {
            break;
          }
        }
        return _matches;
      }
    };
  }
}

#endif // INDRI_LISTNODE_HPP

