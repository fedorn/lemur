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
// UnknownRemoverCopier
// 
// 8 March 2004 -- tds
//
// Makes a copy of a query tree that does not contain
// any fields or terms that don't exist in the index.
// This may propogate up the tree and delete
// proximity operators that will obviously not match
// anything anymore.
//

#ifndef INDRI_UNKNOWNREMOVERCOPIER_HPP
#define INDRI_UNKNOWNREMOVERCOPIER_HPP

#include "indri/Repository.hpp"
#include "indri/QuerySpec.hpp"
#include "indri/Copier.hpp"

class UnknownRemoverCopier : public indri::lang::Copier {
private:
  Repository& _repository;
  std::vector<indri::lang::Node*> _nodes;

  template<class T>
  indri::lang::Node* afterExtentCombine( T* oldNode, T* newNode ) {
    std::vector<indri::lang::RawExtentNode*>& children = newNode->getChildren();
    std::vector<indri::lang::RawExtentNode*> nonNullChildren;

    for( unsigned int i=0; i<children.size(); i++ ) {
      if( children[i] )
        nonNullChildren.push_back( children[i] );
    }

    indri::lang::Node* result;

    if( children.size() == nonNullChildren.size() ) {
      // all children are legit, so there's no need to change this node
      result = newNode;
      _nodes.push_back( result );
    } else if( nonNullChildren.size() >= 2 ) {
      // some of the children aren't legitimate, so make a new node with fewer children
      result = new T( nonNullChildren );
      _nodes.push_back( result );
    } else if( nonNullChildren.size() == 1 ) {
      // only one child remains, so there's no need for an and/or node
      result = nonNullChildren[0];
    } else {
      // no children remain, so propogate 0 up the chain
      result = 0;
    }

    if( result != newNode ) {
      delete newNode;
    }

    return result;
  }


  template<class T>
  indri::lang::Node* afterWindow( T* oldNode, T* newNode ) {
    const std::vector<indri::lang::RawExtentNode*>& children = newNode->getChildren();
    std::vector<indri::lang::RawExtentNode*> nonNullChildren;

    for( unsigned int i=0; i<children.size(); i++ ) {
      if( children[i] )
        nonNullChildren.push_back( children[i] );
    }

    indri::lang::Node* result;

    if( children.size() == nonNullChildren.size() ) {
      // all children are legit, so there's no need to change this node
      result = newNode;
      _nodes.push_back(result);
    } else if( nonNullChildren.size() >= 2 ) {
      // some of the children aren't legitimate, so make a new node with fewer children
      result = new T( newNode->getWindowSize(), nonNullChildren );
      _nodes.push_back(result);
    } else if( nonNullChildren.size() == 1 ) {
      // only one child remains, so there's no need for a window node
      result = nonNullChildren[0];
    } else {
      // no children remain, so propogate 0 up the chain
      result = 0;
    }

    if( result != newNode ) {
      delete newNode;
    }

    return result;
  }

public:
  UnknownRemoverCopier( Repository& repository ) :
    _repository(repository)
  {
  }

  indri::lang::Node* defaultAfter( indri::lang::Node* oldNode, indri::lang::Node* newNode ) {
    _nodes.push_back( newNode );
    return newNode;
  }

  indri::lang::Node* after( indri::lang::IndexTerm* oldTerm, indri::lang::IndexTerm* newTerm ) {
    std::string processed = newTerm->getText();
    if( newTerm->getStemmed() == false )
      processed = _repository.processTerm( newTerm->getText() );

    if( processed.length() == 0 ) {
      delete newTerm;
      newTerm = 0;
    } else {
      _nodes.push_back( newTerm );
    }

    return newTerm;
  }

  indri::lang::Node* after( indri::lang::Field* oldField, indri::lang::Field* newField ) {
    int fieldID = _repository.index()->field( newField->getFieldName().c_str() );

    if( !fieldID ) {
      delete newField;
      newField = 0;
    } else {
      _nodes.push_back( newField );
    }

    return newField;
  }

  indri::lang::Node* after( indri::lang::ExtentAnd* oldNode, indri::lang::ExtentAnd* newNode ) {
    return afterExtentCombine<indri::lang::ExtentAnd>( oldNode, newNode );
  }

  indri::lang::Node* after( indri::lang::ExtentOr* oldNode, indri::lang::ExtentOr* newNode ) {
    return afterExtentCombine<indri::lang::ExtentOr>( oldNode, newNode );
  }

  indri::lang::Node* after( indri::lang::ExtentInside* oldNode, indri::lang::ExtentInside* newNode ) {
    if( newNode->getInner() && newNode->getOuter() ) {
      _nodes.push_back( newNode );
      return newNode;
    } else {
      delete newNode;
      return 0;
    }
  }

  indri::lang::Node* after( indri::lang::ODNode* oldNode, indri::lang::ODNode* newNode ) {
    return afterWindow<indri::lang::ODNode>(oldNode, newNode);
  }

  indri::lang::Node* after( indri::lang::UWNode* oldNode, indri::lang::UWNode* newNode ) {
    return afterWindow<indri::lang::UWNode>(oldNode, newNode);
  }

  indri::lang::Node* after( indri::lang::RawScorerNode* oldNode, indri::lang::RawScorerNode* newNode ) {
    _nodes.push_back( newNode );
    return newNode;
  }

  indri::lang::Node* after( indri::lang::ContextCounterNode* oldNode, indri::lang::ContextCounterNode* newNode ) {
    _nodes.push_back( newNode );
    return newNode;
  }

  indri::lang::Node* after( indri::lang::ScoredExtentNode* oldNode, indri::lang::ScoredExtentNode* newNode ) {
    _nodes.push_back( newNode );
    return newNode;
  }
};

#endif // INDRI_UNKNOWNREMOVERCOPIER_HPP


