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

// Data structure to support fast average case (O(log h)) checking for
// overlapping tags as read in by the OffsetAnnotationsAnnotator.

// The IntervalTree is a binary tree ordered by "begin", the low
// endpoint of the interval.  Each node represents an interval,
// defined by "begin" and "end", and at each node we also store a
// special value "max_child_end", which is the highest end value
// present in any one child of that node.

// No attempt is made to balance the tree as it is grown, so the worst
// case search time could be O(n) in the number of intervals stored in
// the tree.

// Each node of this tree contains an STL map of children, sorted in
// *decreasing* first-and-longest order.  The STL set guarantees that
// insertion is O(log n), and that deletion is O(1) when we are
// holding an iterator that points to the element to be removed.

// The insertion algorithm is as follows.  It is recursive beginning
// from the root of the tree.

// First, if we are inserting an interval that is already in the tree,
// we return true, since it is ok for an interval to have more than
// one tag on it (eg. <b><i>foo</i></b>).

// Next, we check for an enclosure condition in which the interval
// being added to the tree actually encloses the node we are currently
// examining.  In this case, the node is removed, and re-inserted as a
// child of the interval being added.  True is returned in this case.
// Currently this only works when the current node has no children.

// Next, the interval is checked for overlapping against the current
// node.  If there is an overlap, false is returned.

// Finally, the recursive step.  If the begin value of the interval
// being inserted is less than or equal to the begin value of the
// current node, we recurse to the left child.  Otherwise, we recurse
// to the right child.  Prior to recursing to the left child,
// max_child_end is checked to ensure that storing the interval will
// not result in an overlap.

// After any insertion, changes to max_child_end are propagated all
// the way up to the root of the tree.

#include <iostream>
#include <string>
#include "indri/IntervalTree.hpp"

namespace indri {
  namespace parse {

    // This member function returns false if there was a failure
    // adding the Interval to the data structure, or if an overlapping
    // tag constraint was violated.  True is returned otherwise.

    bool IntervalTree::insert( int begin, int end ) {

      if ( ! root ) {

        root = new IntervalTreeNode( begin, end );
        return true;
      }

      return _insert( begin, end, root );
    }

    bool IntervalTree::_insert( int begin, int end, IntervalTreeNode* node ) {

      // Check if interval is equal to current node

      if ( begin == node->begin && end == node->end ) return true;

      // Check for enclosure

      if ( begin <= node->begin && end >= node->max_child_end ) {

        // This interval encloses the entire subtree rooted at this
        // node.

        IntervalTreeNode *a = new IntervalTreeNode( begin, end );

        if ( node->begin < end ) { // left insertion

          a->left_child = node;

        } else { // right insertion

          a->right_child = node;
        }

        if ( node->parent->right_child == node )
          node->parent->right_child = a;
        else if ( node->parent->left_child == node )
          node->parent->left_child = a;

        a->parent = node->parent;
        node->parent = a;

        // Properly attach children
        a->left_child = node->left_child;
        a->right_child = node->right_child;

        int mce = a->max_child_end > node->max_child_end ?
          a->max_child_end : node->max_child_end;

        // Recursively update max_child_end
        IntervalTreeNode *par = node;

        while ( par ) {

          if ( par->max_child_end < mce ) par->max_child_end = mce;
          par = par->parent;
        }
        return true;
      }

      // Check for overlap

      if ( ( begin < node->end && node->end < end ) ||
           ( begin < node->begin && node->begin < end ) ) {

        std::cout << "Overlap detected." << std::endl;
        return false;
      }

      // No overlap at this node so recurse:

      if ( begin < node->end ) { // left recursion

        if ( node->left_child ) {

//        if ( node->left_child->max_child_end <= end ) return false;
          if ( end > node->end ) return false;
          else return _insert( begin, end, node->left_child );

        } else {

          // Do the insertion
          node->left_child = new IntervalTreeNode( begin, end );
          node->left_child->parent = node;

          // Recursively update max_child_end
          IntervalTreeNode *par = node;

          while ( par ) {

            if ( par->max_child_end < end ) par->max_child_end = end;
            par = par->parent;
          }

          return true;
        }

      } else { // right recursion

        if ( node->right_child ) {

          // No max_child_end check here
          return _insert( begin, end, node->right_child );

        } else {

          // Do the insertion
          node->right_child = new IntervalTreeNode( begin, end );
          node->right_child->parent = node;

          // Recursively update max_child_end
          IntervalTreeNode *par = node;

          while ( par ) {

            if ( par->max_child_end < end ) par->max_child_end = end;
            par = par->parent;
          }

          return true;
        }

      }

      return false;
    }

    void IntervalTree::walk_tree( std::ostream& s ) {

      _walk_tree( s, root, 0 );
    }

    void IntervalTree::_walk_tree( std::ostream& s, IntervalTreeNode* node,
                                   int indent ) {

      std::string ind( indent, ' ' );
      bool children = false;

      s << ind << "[<" << node->begin << ", " << node->end << ", "
        << node->max_child_end << ">";

      if ( node->left_child ) {
        s << std::endl << ind << "left" << std::endl;
        _walk_tree( s, node->left_child, indent + 2 );
        children = true;
      }

      if ( node->right_child ) {
        s <<  std::endl << ind << "right" << std::endl;
        _walk_tree( s, node->right_child, indent + 2 );
        children = true;
      }

      if ( children ) s << std::endl << ind;
      s << "]" << std::endl;
    }
  }
}
