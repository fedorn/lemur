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
// HashTable
//
// 9 January 2004 - tds
//

#ifndef LEMUR_HASHTABLE_HPP
#define LEMUR_HASHTABLE_HPP

#include <utility>

//
// GenericHash<_Key>
//

template<class _Key>
class GenericHash { 
public:
  int operator() ( const _Key& k ) const {
    return (int) k;
  }
};

//
// GenericComparator<_Key>
//

template<class _Key>
class GenericComparator {
public:
  int operator() ( const _Key& one, const _Key& two ) const {
    return (int) (one - two);
  }
};

template<>
class GenericHash<const char*> {
public:
  int operator() ( const char* const& kp ) const {
    int hash = 0;
    const char* k = kp;
    for( ; *k; k++ ){
      hash *= 7;
      hash += *k;
    }
    return hash;
  }
};

template<>
class GenericComparator<const char*> {
public:
  int operator () ( const char* const& one, const char* const& two ) const {
    return strcmp( one, two );
  }
};

//
// HashBucket<_Key, _Value>
//

template<class _Key, class _Value>
struct HashBucket {
  _Key key;
  _Value value;
  HashBucket<_Key, _Value>* next;

  HashBucket() : next( (HashBucket<_Key, _Value>*) ~0 ) {};
  HashBucket( const _Key& k, HashBucket<_Key, _Value>* n ) : key(k), next(n) {};
  HashBucket( const _Key& k, const _Value& v, HashBucket<_Key, _Value>* n ) : key(k), value(v), next(n) {};
  
  ~HashBucket() {
    next = (HashBucket<_Key, _Value>*) ~0;
  }

  bool empty() {
    return next == (HashBucket<_Key, _Value>*) ~0;
  }

  void setEmpty() {
    next = (HashBucket<_Key, _Value>*) ~0;
  }
};

//
// HashTableIterator<_Key, _Value, _Comparator>
//

template<class _Key, class _Value, class _Comparator>
class HashTableIterator {
private:
  typedef HashBucket<_Key, _Value> bucket_type;
  bucket_type* _table;
  bucket_type* _currentEntry;
  size_t _currentBucket;
  size_t _totalBuckets;
  std::pair<_Key*, _Value*> _pair;

  void next() {
    // already at end
    if( _currentBucket == -1 )
      return;

    // in a chain with more entries left
    if( _currentEntry && _currentEntry->next != 0 ) {
      _currentEntry = _currentEntry->next;
      return;
    }

    if( _currentEntry )
      _currentBucket++;

    for( ; _currentBucket < _totalBuckets; _currentBucket++ ) {
      _currentEntry = &_table[_currentBucket];

      if( ! _currentEntry->empty() ) {
        return;
      }
    }

    // none left
    _currentEntry = 0;
    _currentBucket = -1;
  }

public:
  HashTableIterator() {
    _currentBucket = -1;
    _currentEntry = 0;
  }

  HashTableIterator( bucket_type* table, size_t totalBuckets ) {
    _table = table;
    _totalBuckets = totalBuckets;

    _currentBucket = 0;
    _currentEntry = 0;
    next();
  }

  bool operator == ( const HashTableIterator& other ) {
    if( other._currentEntry == _currentEntry &&
        other._currentBucket == _currentBucket )
        return true;
    return false;
  }

  bool operator != ( const HashTableIterator& other ) {
    if( other._currentEntry == _currentEntry &&
        other._currentBucket == _currentBucket )
        return false;
    return true;
  }

  void operator++ ( int ) {
    next();
  }

  std::pair<_Key*, _Value*>& operator* () {
    _pair.first = &_currentEntry->key;
    _pair.second = &_currentEntry->value;
    return _pair;
  }

  std::pair<_Key*, _Value*>* operator-> () {
    return &(*(*this));
  }
};

template<class _Key, class _Value, class _HashFunction = GenericHash<_Key>, class _Comparator = GenericComparator<_Key> >
class HashTable {
public:
  friend class HashTableIterator<_Key, _Value, _Comparator>;

  typedef HashBucket<_Key, _Value> bucket_type;
  typedef _Key key_type;
  typedef _Value value_type;
  typedef _HashFunction hash_type;
  typedef _Comparator compare_type;
  typedef class HashTableIterator<_Key, _Value, _Comparator> iterator;

private:
  bucket_type* _table;
  hash_type _hash;
  compare_type _compare;
  size_t _buckets;
  iterator _end;
  size_t _count;

  bucket_type* _parentBucket( const _Key& k ) const {
    size_t index = _hash(k) % _buckets;
    return &_table[index];
  }

public:
  HashTable( size_t size = 16384 ) {
    _buckets = size / sizeof(bucket_type);
    _table = reinterpret_cast<bucket_type*>(new char[_buckets * sizeof(bucket_type)]);
    _count = 0;
    
    for( size_t i=0; i<_buckets; i++ ) {
      _table[i].setEmpty();
    }
  }

  ~HashTable() {
    clear();
    delete[] reinterpret_cast<char*>(_table);
  }

  _Value* find( const _Key& k ) const {
    bucket_type* bucket = _parentBucket(k);

    if( bucket->empty() ) {
      return 0;
    } else {
      while(1) {
        if( _compare( k, bucket->key ) == 0 ) {
            return &bucket->value;
        }
        
        if( bucket->next == 0 )
          return 0;
        else
          bucket = bucket->next;
      }
    }
  }

  _Value* insert( const _Key& k ) {
    bucket_type* bucket = _parentBucket(k);
    _count++;

    if( bucket->empty() ) {
      new(bucket) bucket_type( k, 0 );
      return &bucket->value;
    } else {
      bucket_type* newBucket = new bucket_type( k, bucket->next );
      bucket->next = newBucket;
      return &newBucket->value;
    }
  }

  _Value* insert( const _Key& k, const _Value& v ) {
    bucket_type* bucket = _parentBucket(k);
    _count++;

    if( bucket->empty() ) {
      new(bucket) bucket_type( k, v, 0 );
      return &bucket->value;
    } else {
      bucket_type* newBucket = new bucket_type( k, v, bucket->next );
      bucket->next = newBucket;
      return &newBucket->value;
    }
  }

  void remove( const _Key& k ) {
    bucket_type* bucket = _parentBucket(k);

    if( !bucket->empty() ) {
      if( _compare( k, bucket->key ) == 0 ) {
        if( bucket->next ) {
          bucket_type* nextBucket = bucket->next;
          bucket->~bucket_type();
          new(bucket) bucket_type( nextBucket->key, nextBucket->value, nextBucket->next );
          delete nextBucket;
        } else {
          bucket->~bucket_type();
        }
        _count--;
      } else {
        bucket_type* parent = bucket;
        bucket = parent->next;

        while( bucket ) {
          if( _compare( k, bucket->key ) == 0 ) {
            _count--;
            parent->next = bucket->next;
            delete bucket;
            break;
          }
          parent = bucket;
          bucket = bucket->next;
        }
      }
    }
  }

  void clear() {
    for( size_t i=0; i<_buckets; i++ ) {
      if( !_table[i].empty() ) {
        bucket_type* current = _table[i].next;
        bucket_type* next;

        _table[i].~bucket_type();

        while( current ) {
          next = current->next;
          delete current;
          current = next;
        }
      }
    }
    _count = 0;
  }

  const iterator& end() {
    return _end;
  }

  iterator begin() {
    return iterator( _table, _buckets );
  }

  size_t size() {
    return _count;
  }
};

#endif // LEMUR_HASHTABLE_HPP
