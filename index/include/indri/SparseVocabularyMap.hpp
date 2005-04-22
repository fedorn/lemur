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
// SparseVocabularyMap
//
// 26 November 2004 -- tds
//

#ifndef INDRI_SPARSEVOCABULARYMAP_HPP
#define INDRI_SPARSEVOCABULARYMAP_HPP

//
// Status: this isn't complete yet--still need to figure out the right
// interface, etc.  The bitmap idea seems good, though
//
namespace indri
{
  namespace index
  {
    
    class SparseVocabularyMap {
    public:
      struct map_entry {
        int baseID;
        int type;
        union {
          int* dense;
          int* bitmap;
        };
      };

      std::vector<void*> _map;

      int operator[] ( int oldTermID ) {
        map_entry& entry = _map[oldTermID >> 8];
        static const int bits[] = {
          0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
          1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
          1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
          2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
          1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
          2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
          2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
          3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
          1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
          2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
          2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
          3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
          2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
          3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
          3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
          4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
        };

        if( entryType == 0 ) {
          // ultra-sparse: no bitmap necessary, because all bits are set
          return entry.baseID + (oldTermID & 0xff);
        } else if( entry.type == 1 ) {
          // sparse: scan bitmap, accumulating entries until we know the appropriate termID
          int leadingBits;
          for( int i=0; i < (oldTermID&0xff)/8; i++ ) {
            leadingBits += bits[ entry.bitmap[i] ];
          }

          // TODO: add more here
        } else { // if( entry.type == 2 ) {
          // dense:
          return entry.dense[oldTermID - oldTermID>>8];
        }
      }
    };
  }
}

#endif
