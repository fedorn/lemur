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
// delete_range
//
// 5 March 2004 -- tds
//

#ifndef LEMUR_DELETE_RANGE_HPP
#define LEMUR_DELETE_RANGE_HPP

#include <vector>
#include <stack>
#include <map>

template<typename VectorType>
void delete_vector_contents( std::vector<VectorType>& vector ) {
  typedef typename std::vector<VectorType>::iterator vec_iter;

  vec_iter begin = vector.begin();
  vec_iter end = vector.end();
  vec_iter iter;

  for( iter = begin; iter != end; iter++ ) {
    delete *iter;
  }

  vector.clear();
}

template<typename MapKey, typename MapValue>
void delete_map_contents( std::map<MapKey, MapValue*>& mp ) {
  typedef typename std::map<MapKey, MapValue*>::iterator map_iter;

  map_iter begin = mp.begin();
  map_iter end = mp.end();
  map_iter iter;

  for( iter = begin; iter != end; iter++ )
    delete iter->second;

  mp.clear();
}

template<typename VectorType>
class VectorDeleter {
private:
  std::vector<VectorType>* _vec;

public:
  VectorDeleter() : _vec(0) {}
  VectorDeleter( std::vector<VectorType>& vec ) : _vec(&vec) {}
  ~VectorDeleter() {
    if( _vec )
      delete_vector_contents<VectorType>( *_vec );
  }

  void setVector( std::vector<VectorType>& vec ) {
    _vec = &vec;
  }
};

template<typename StackType>
class StackDeleter {
private:
  std::stack<StackType*>* _stack;

public:
  StackDeleter() : _stack(0) {}
  StackDeleter( std::stack<StackType*>& stack ) : _stack(&stack) {}
  ~StackDeleter() {
    if( _stack ) {
      while( _stack->size() ) {
        delete _stack->top();
        _stack->pop();
      }
    }
  }

  void setStack( std::stack<StackType*>& stack ) {
    _stack = &stack;
  }
};

#endif // LEMUR_DELETE_RANGE_HPP

