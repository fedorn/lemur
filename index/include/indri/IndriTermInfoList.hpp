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
// IndriTermInfoList.hpp
//
// 13 August 2004 -- tds
//

#ifndef INDRI_TERMINFOLIST_HPP
#define INDRI_TERMINFOLIST_HPP

#include "InvFPTypes.hpp"
#include "TermInfoList.hpp"


namespace indri {
  namespace index {
    class BagList : public ::TermInfoList {
    private:
      class TermListBuilder* _list;
      std::vector< std::pair<int, int> > _termCounts;
      TermInfo _info;
      int _position;

    public:
      BagList( class TermListBuilder *list ) ;
      
      ~BagList() ;

      void startIteration() const ;
      bool hasMore() const ;
      TermInfo* nextEntry() const ;

      // Note: even though we have position information here, I'm not returning it
      //       if you really really want it, put in a feature request or something
      TermInfo* getElement( TermInfo* elem, POS_T position ) const ;

      POS_T beginPosition() const ;

      POS_T endPosition() const ;

      POS_T nextPosition( POS_T position ) const ;
    };

    class PositionInfo : public ::TermInfo {
    private:
      int _position;

    public:
      void position( int p ) {
        _position = p;
      }

      int position() {
        return _position;
      }
    };

    class PositionList : public ::TermInfoList {
    private:
      class TermListBuilder* _list;
      PositionInfo _info;
      int _position;
      
    public:
      PositionList( class TermListBuilder* list ) ;
      ~PositionList() ;
      
      TermInfo* newElement() ;

      void assignElement( TermInfo* to, TermInfo* from ) ;

      TermInfo* getElement( TermInfo* elem, POS_T position ) const ;

      POS_T beginPosition() const ;

      POS_T endPosition() const ;

      POS_T nextPosition( POS_T position ) const ;

      void startIteration() const ;

      bool hasMore() const ;

      TermInfo* nextEntry() const ;
    };
  };
}

#endif // INDRI_TERMINFOLIST_HPP

