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
    class BagList : public lemur::api::TermInfoList {
    private:
      const class TermList* _list;
      std::vector< std::pair<int, int> > _termCounts;
      lemur::api::TermInfo _info;
      int _position;

    public:
      BagList( const class TermList *list ) ;
      
      ~BagList() ;

      void startIteration() const ;
      bool hasMore() const ;
      lemur::api::TermInfo* nextEntry() const ;

      // Note: even though we have position information here, I'm not returning it
      //       if you really really want it, put in a feature request or something
      lemur::api::TermInfo* getElement( lemur::api::TermInfo* elem, lemur::api::POS_T position ) const ;

      lemur::api::POS_T beginPosition() const ;

      lemur::api::POS_T endPosition() const ;

      lemur::api::POS_T nextPosition( lemur::api::POS_T position ) const ;
    };

    class PositionInfo : public lemur::api::TermInfo {
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

    class PositionList : public lemur::api::TermInfoList {
    private:
      const class TermList* _list;
      PositionInfo _info;
      int _position;
      
    public:
      PositionList( const class TermList* list ) ;
      ~PositionList() ;
      
      lemur::api::TermInfo* newElement() ;

      void assignElement( lemur::api::TermInfo* to, lemur::api::TermInfo* from ) ;

      lemur::api::TermInfo* getElement( lemur::api::TermInfo* elem, lemur::api::POS_T position ) const ;

      lemur::api::POS_T beginPosition() const ;

      lemur::api::POS_T endPosition() const ;

      lemur::api::POS_T nextPosition( lemur::api::POS_T position ) const ;

      void startIteration() const ;

      bool hasMore() const ;

      lemur::api::TermInfo* nextEntry() const ;
    };
  }
}

#endif // INDRI_TERMINFOLIST_HPP

