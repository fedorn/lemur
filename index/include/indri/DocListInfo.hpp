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

