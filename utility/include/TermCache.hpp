/*==========================================================================
 * Copyright (c) 2003 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// TermCache
//
// 19 September 2003 - tds
//

#ifndef _LEMUR_TERMCACHE_HPP
#define _LEMUR_TERMCACHE_HPP
namespace lemur 
{
  namespace utility 
  {
    
#define TERMCACHE_MAX_TERM_LENGTH 12
#define TERMCACHE_SIZE    (64 * 1024 * 1024)
#define TERMCACHE_BUCKETS (TERMCACHE_SIZE/16)
    ///External to internal term id hash table for optimizing indexing with
    /// KeyfileIncIndex.
    class TermCache {
    public:
      TermCache();
      /// add a term to the cache.
      void add( const char* term, int termID );
      /// lookup a term in the cache. Returns internal termid or -1 if not found.
      int find( const char* term ) const;
      /// clear the cache.
      void clear();

    private:
      int _hashFunction( const char* term ) const;

      struct cache_entry {
        char term[TERMCACHE_MAX_TERM_LENGTH];
        int id;
      };

      struct cache_entry _termCache[ TERMCACHE_SIZE / sizeof (struct cache_entry) ];
    };
  }
}

#endif // _LEMUR_TERMCACHE_HPP
