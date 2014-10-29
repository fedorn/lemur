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


#include "TermCache.hpp"
#include <string.h>

lemur::utility::TermCache::TermCache() {
  clear();
}

//
// lemur::utility::TermCache::add
//
// The assumption here is that if a term has already been added, it's
// probably more popular than this one, so there's no need to evict.
// Associativity might be nice, but we'd have to show that it's fast.
//

void lemur::utility::TermCache::add( const char* term, int termID ) {
  // don't cache terms longer than TERMCACHE_MAX_TERM_LENGTH
  if (term && (strlen(term) > TERMCACHE_MAX_TERM_LENGTH)) return;

  int bucket = _hashFunction( term );
  
  if( bucket >= 0 ) {
    if( _termCache[bucket].id == -1 ) {
      strncpy( _termCache[bucket].term, term, sizeof _termCache[bucket].term );
      _termCache[bucket].id = termID;
    }
  }
}

//
// lemur::utility::TermCache::find
//

int lemur::utility::TermCache::find( const char* term ) const {
  int bucket = _hashFunction( term );

  if( _termCache[bucket].id != -1 ) {
    if( !strncmp( term, _termCache[bucket].term, sizeof _termCache[bucket].term ) ) {
      return _termCache[bucket].id;
    }
  }

  return -1;
}

//
// lemur::utility::TermCache::clear()
//

void lemur::utility::TermCache::clear() {
  memset( _termCache, 0xff, sizeof _termCache );
}

//
// lemur::utility::TermCache::_hashFunction
//

int lemur::utility::TermCache::_hashFunction( const char* term ) const {
  int hash = 0;
  int index;
  
  for( index = 0; term[index]; index++ )
    hash = 7*hash + term[index];

  if( index <= sizeof( _termCache[0].term ) )
    return hash % ( TERMCACHE_SIZE / sizeof( struct cache_entry ) );
  
  return -1;
}

