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
// DocumentVector
//
// 26 July 2004 -- tds
//

#include "indri/DocumentVector.hpp"
#include "indri/IndriIndex.hpp"
#include "indri/TermListBuilder.hpp"

void DocumentVector::_init( class IndriIndex* index, const indri::index::TermListBuilder* termList, std::map<int,std::string>* termStringMap ) {
  const greedy_vector<int>& terms = termList->terms();
  const greedy_vector<indri::index::FieldExtent>& fields = termList->fields();

  // build a vocabulary
  std::map<int, int> termIDMap;
  _stems.push_back( "[OOV]" );

  for( unsigned int i=0; i<terms.size(); i++ ) {
    if( terms[i] == 0 ) {
      _positions.push_back(0);
    } else {
      std::map<int, int>::iterator iter = termIDMap.find( terms[i] );
      
      if( iter != termIDMap.end() ) {
        // we've seen this stem, add its position
        _positions.push_back(iter->second);
      } else {
        std::string termString;
        std::map<int,std::string>::iterator siter;
        
        // the term string map may be passed in from a caller that's fetching a bunch
        // of doc vectors at once--the cache can help us find terms a little faster
        if( termStringMap && (siter = termStringMap->find(terms[i])) != termStringMap->end() ) {
          termString = siter->second;
        } else {
          termString = index->term(terms[i]);
        }

        _stems.push_back( termString );
        _positions.push_back(_stems.size()-1);
        termIDMap[terms[i]] = _stems.size()-1;

        // put the string in the termStringMap for future DocumentVector builders
        if( termStringMap && siter == termStringMap->end() ) {
          (*termStringMap)[terms[i]] = termString;
        }
      }
    }
  }

  for( unsigned int i=0; i<fields.size(); i++ ) {
    Field f;
    f.name = index->field(fields[i].id);
    f.number = fields[i].number;
    f.begin = fields[i].begin;
    f.end = fields[i].end;

    _fields.push_back(f);
  }
}

DocumentVector::DocumentVector() {
}

DocumentVector::DocumentVector( IndriIndex* index, const indri::index::TermListBuilder* termList ) {
  _init( index, termList, 0 );
}

DocumentVector::DocumentVector( IndriIndex* index, const indri::index::TermListBuilder* termList, std::map<int,std::string>& termStringMap ) {
  _init( index, termList, &termStringMap );
}

const std::vector<std::string>& DocumentVector::stems() const {
  return _stems;
}

std::vector<std::string>& DocumentVector::stems() {
  return _stems;
}

const std::vector<int>& DocumentVector::positions() const {
  return _positions;
}

std::vector<int>& DocumentVector::positions() {
  return _positions;
}

const std::vector<DocumentVector::Field>& DocumentVector::fields() const {
  return _fields;
}

std::vector<DocumentVector::Field>& DocumentVector::fields() {
  return _fields;
}

