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


