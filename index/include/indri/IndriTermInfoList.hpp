/*==========================================================================
  Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.

  Use of the Lemur Toolkit for Language Modeling and Information Retrieval
  is subject to the terms of the software license set forth in the LICENSE
  file included with this software, and also available at
  http://www.cs.cmu.edu/~lemur/license.html 
  as well as the conditions below.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in
  the documentation and/or other materials provided with the
  distribution.

  3. The names "Indri", "Center for Intelligent Information Retrieval", 
  "CIIR", and "University of Massachusetts" must not be used to
  endorse or promote products derived from this software without
  prior written permission. To obtain permission, contact
  indri-info@ciir.cs.umass.edu.

  4. Products derived from this software may not be called "Indri" nor 
  may "Indri" appear in their names without prior written permission of 
  the University of Massachusetts. To obtain permission, contact 
  indri-info@ciir.cs.umass.edu.

  THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY OF MASSACHUSETTS AND OTHER
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGE.
  ==========================================================================
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

