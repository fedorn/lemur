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
// DocListInfo
//
// 9 January 2004 - tds
//

#ifndef INDRI_DOCLISTINFO_HPP
#define INDRI_DOCLISTINFO_HPP

#include "indri/DocPositionInfoList.hpp"
#include <indri/greedy_vector>

///
/// This is the DocInfo structure that Indri
/// indexes return from the docInfoList() and
/// docPositionInfoList() calls.  This structure is
/// also used internally by IndriIndex when merging
/// inverted lists.
///

namespace indri {
  namespace index {
    class DocListInfo : public DocInfo {
    private:
      DOCID_T _documentID;
      TERMID_T _termID;
      greedy_vector<LOC_T,32> _positions;

    public:
      // DocInfo interface
      DOCID_T docID() const {
        return _documentID;
      }

      COUNT_T termCount() const {
        return COUNT_T(_positions.size());
      }

      virtual const LOC_T* positions() const {
        return &_positions.front();
      }

      // Internal index use
      TERMID_T termID() const {
        return _termID;
      }

      void setDocID( DOCID_T documentID ) {
        _documentID = documentID;
      }

      void setTermID( TERMID_T termID ) {
        _termID = termID;
      }

      void addPosition( LOC_T position ) {
        _positions.push_back( position );
      }

      void addPositions( LOC_T* positions, int count ) {
        _positions.append( positions, positions + count );
      }

      void clear() {
        _positions.clear();
      }
    };
  }
}

#endif // KEYFILE_DOCLISTINFO_HPP

