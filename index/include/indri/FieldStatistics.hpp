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
// FieldStatistics
//
// 4 February 2004 -- tds
//

#ifndef INDRI_FIELDSTATISTICS_HPP
#define INDRI_FIELDSTATISTICS_HPP

namespace indri {
  namespace index {
    struct FieldStatistics {
      FieldStatistics( const std::string& _name, bool numeric )
        :
        name(_name),
        isNumeric(numeric),
        totalCount(0),
        documentCount(0),
        lastDocument(0),
        lastCount(0)
      {
      }

      FieldStatistics( const std::string& _name, bool numeric, UINT64 _totalCount, unsigned int _documentCount )
        :
        name(_name),
        isNumeric(numeric),
        totalCount(_totalCount),
        documentCount(_documentCount),
        lastDocument(0),
        lastCount(0) {
      }

      void addOccurrence( int documentID ) {
        if( documentID != lastDocument ) {
          lastCount = 0;
          lastDocument = documentID;
          documentCount++;
        }

        totalCount++;
        lastCount++;
      }
      
      std::string name;
      bool isNumeric;
      UINT64 totalCount;
      unsigned int documentCount;

      int lastDocument;
      int lastCount;
    };
  }
}

#endif // INDRI_FIELDSTATISTICS_HPP

