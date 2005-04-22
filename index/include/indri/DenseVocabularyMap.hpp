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
// DenseVocabularyMap
//
// 29 November 2004 -- tds
//

#ifndef INDRI_DENSEVOCABULARYMAP_HPP
#define INDRI_DENSEVOCABULARYMAP_HPP
namespace indri
{
  namespace index
  {
    
    class DenseVocabularyMap {
    private:
      std::vector<int> _map;

    public:
      int store( int oldTermID, int newTermID ) {
        if( _map.size() < oldTermID )
          _map.resize( oldTermID+1 );

        _map[oldTermID] = newTermID;
      }

      int operator[] ( int oldTermID ) {
        return get( oldTermID );
      }

      int get( int oldTermID ) {
        return _map[oldTermID];
      }
    };
  }
}

#endif // INDRI_DENSEVOCABULARYMAP_HPP

