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
// ExtentRestrictionModelAnnotatorCopier
//
// 16 August 2004 -- tds
//
// This copier annotates RawScorerNodes with the language models
// of the surrounding ExtentRestrictions.
//
// For example, the query #combine[sentence]( dog cat )
// should be synonymous with #combine[sentence]( dog.(sentence) cat.(sentence) ).
// We push the sentence language model down the tree and attach it to "dog" and "cat".
//

#ifndef INDRI_EXTENTRESTRICTIONMODELANNOTATORCOPIER_HPP
#define INDRI_EXTENTRESTRICTIONMODELANNOTATORCOPIER_HPP

#include <stack>
#include <vector>
#include "indri/QuerySpec.hpp"

class ExtentRestrictionModelAnnotatorCopier : public indri::lang::Copier {
private:
  std::vector<indri::lang::Node*> _nodes;
  std::stack< indri::lang::ExtentRestriction* > _restrictions;

public:
  ~ExtentRestrictionModelAnnotatorCopier() {
    delete_vector_contents( _nodes );
  }

  indri::lang::Node* defaultAfter( indri::lang::Node* old, indri::lang::Node* newNode ) {
    _nodes.push_back( newNode );
    return newNode;
  }

  void before( indri::lang::ExtentRestriction* old ) {
    _restrictions.push( old );
  }

  indri::lang::Node* after( indri::lang::ExtentRestriction* oldNode, indri::lang::ExtentRestriction* newNode ) {
    _restrictions.pop();
    _nodes.push_back( newNode );
    return newNode;
  }
  
  indri::lang::Node* after( indri::lang::RawScorerNode* oldNode, indri::lang::RawScorerNode* newNode ) {
    if( newNode->getContext() == 0 && _restrictions.size() ) {
      newNode->setContext( _restrictions.top()->getField() );
    }

    return newNode;
  }
};

#endif // INDRI_EXTENTRESTRICTIONMODELANNOTATORCOPIER_HPP

