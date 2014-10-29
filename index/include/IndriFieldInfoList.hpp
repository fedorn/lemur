/*==========================================================================
 * Copyright (c) 2007 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 *
 * 26 Sep 2007 - mjh - initial file start
 */


#ifndef _INDRIFIELDINFOLIST_HPP
#define _INDRIFIELDINFOLIST_HPP

#include "FieldInfoList.hpp"
#include "indri/FieldExtent.hpp"
#include "indri/greedy_vector"
#include <vector>

namespace lemur
{
  namespace api 
  {
    
    /*!

    FieldInfo list class for Indri-style indexes

    */

    class IndriFieldInfoList : public FieldInfoList {
    protected:
      int _position;
      std::vector<lemur::api::FieldInfo> _list;
      lemur::api::FieldInfo _currentInfo;

    public:
      virtual ~IndriFieldInfoList();

    protected:
      /// Set element from position, returns pointer to the element
      virtual FieldInfo* getElement(FieldInfo* elem, POS_T position) const;

      /// Position at beginning of list
      virtual POS_T beginPosition() const;

      /// Position at end of list
      virtual POS_T endPosition() const;

      /// Advance to next position
      virtual POS_T nextPosition(POS_T position) const;

    public:
      /// C'tor - initializes a blank list
      IndriFieldInfoList();

      /// C'tor - initializes a list with field extents
      IndriFieldInfoList(const indri::utility::greedy_vector< indri::index::FieldExtent > &fields);

      /// appends a new field entry at the end of the list
      void add(indri::index::FieldExtent newExtent);

      // Single, internal iteration
      /// Prepare iteration
      virtual void startIteration();

      /// Has more entries
      virtual bool hasMore() const;

      /// Get a pointer to the next entry (pointer to a local static memory), so do not delete it
      virtual FieldInfo *nextEntry() const;

      /// returns the number of FieldInfo elements in the list
      virtual int size();

      /// indexed access to the list
      virtual FieldInfo* operator[] (int index);

    }; // end class IndriFieldInfoList
  } // end namespace api 
} // end namespace lemur


#endif
