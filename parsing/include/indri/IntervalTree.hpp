/*==========================================================================
 * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

//
// IntervalTree
//
// 15 September 2005 -- mwb
//

// See src/IntervalTree.cpp for more documentation.

#ifndef INDRI_INTERVALTREE_HPP
#define INDRI_INTERVALTREE_HPP

#include <set>
#include <utility>

namespace indri {
  namespace parse {
    
    class IntervalTreeNode {

    public:

      int begin;
      int end;
      int max_child_end;

      IntervalTreeNode *left_child;
      IntervalTreeNode *right_child;
      IntervalTreeNode *parent;

      IntervalTreeNode( int begin = 0, int end = 0 ) {

	left_child = NULL;
	right_child = NULL;
	parent = NULL;

	this->begin = begin;
	this->end = end;
	max_child_end = end;
      }

      ~IntervalTreeNode() {
	
	if ( left_child ) delete left_child;
	if ( right_child ) delete right_child;
      }

    };
    
    class IntervalTree {

    private:
      IntervalTreeNode* root;
      bool _insert( int begin, int end, IntervalTreeNode* node );

    public:
      IntervalTree() { root = NULL; }
      ~IntervalTree() { if ( root ) delete root; }

      bool insert( int begin, int end );

    };

  }
}

#endif // INDRI_INTERVALTREE_HPP
