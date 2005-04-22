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
// VocabularyMap
//
// 29 November 2004 -- tds
//

#ifndef INDRI_VOCABULARYMAP_HPP
#define INDRI_VOCABULARYMAP_HPP

namespace indri {
  namespace index {
    class VocabularyMap {
    public:
      virtual ~VocabularyMap() = 0;

      virtual void store( int oldTermID, int newTermID ) = 0;
      virtual int get( int oldTermID ) = 0;
      virtual int operator[] ( int oldTermID ) = 0;
    };
  }
}

#endif // INDRI_VOCABULARYMAP_HPP


