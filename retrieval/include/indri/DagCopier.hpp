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
// DagCopier
//
// 5 March 2004 -- tds
//

#ifndef INDRI_DAGCOPIER_HPP
#define INDRI_DAGCOPIER_HPP

#include <vector>
#include "indri/delete_range.hpp"

class DagCopier : public indri::lang::Copier {
private:
  std::vector<indri::lang::IndexTerm*> _terms;
  std::vector<indri::lang::Field*> _fields;
  std::vector<indri::lang::ExtentOr*> _extentOrs;
  std::vector<indri::lang::ExtentAnd*> _extentAnds;
  std::vector<indri::lang::ExtentInside*> _extentInsides;
  std::vector<indri::lang::ODNode*> _odNodes;
  std::vector<indri::lang::UWNode*> _uwNodes;

  std::vector<indri::lang::Node*> _newNodes;

  template<class T>
  T* _findReplacement( std::vector<T*>& replacements, T* candidate ) {
    T* replacement = 0;
    
    for( unsigned int i=0; i<replacements.size(); i++ ) {
      if( (*candidate) == (*replacements[i]) ) {
        replacement = replacements[i];
        break;
      }
    }

    if( replacement ) {
      delete candidate;
      candidate = replacement;
    } else {
      _newNodes.push_back( candidate );
      replacements.push_back( candidate );
    }

    return candidate;
  }

public:
  ~DagCopier() {
    delete_vector_contents( _newNodes );
  }

  indri::lang::Node* defaultAfter( indri::lang::Node* oldNode, indri::lang::Node* newNode ) {
    _newNodes.push_back( newNode );
    return newNode;
  }

  indri::lang::Node* after( indri::lang::IndexTerm* indexTerm, indri::lang::IndexTerm* newIndexTerm ) {
    return _findReplacement<indri::lang::IndexTerm>( _terms, newIndexTerm );
  }

  indri::lang::Node* after( indri::lang::Field* field, indri::lang::Field* newField ) {
    return _findReplacement<indri::lang::Field>( _fields, newField );
  }

  indri::lang::Node* after( indri::lang::ExtentOr* oldExtentOr, indri::lang::ExtentOr* newExtentOr ) {
    return _findReplacement<indri::lang::ExtentOr>( _extentOrs, newExtentOr );
  }

  indri::lang::Node* after( indri::lang::ExtentAnd* oldExtentAnd, indri::lang::ExtentAnd* newExtentAnd ) {
    return _findReplacement<indri::lang::ExtentAnd>( _extentAnds, newExtentAnd );
  }

  indri::lang::Node* after( indri::lang::ExtentInside* oldExtentInside, indri::lang::ExtentInside* newExtentInside ) {
    return _findReplacement<indri::lang::ExtentInside>( _extentInsides, newExtentInside );
  }

  indri::lang::Node* after( indri::lang::ODNode* oldODNode, indri::lang::ODNode* newODNode ) {
    return _findReplacement<indri::lang::ODNode>( _odNodes, newODNode );
  }

  indri::lang::Node* after( indri::lang::UWNode* oldUWNode, indri::lang::UWNode* newUWNode ) {
    return _findReplacement<indri::lang::UWNode>( _uwNodes, newUWNode );
  }
};

#endif // INDRI_DAGCOPIER_HPP

