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
// UnnecessaryNodeRemoverCopier
//
// 17 March 2004 -- tds
//

#ifndef INDRI_UNNECESSARYNODEREMOVERCOPIER_HPP
#define INDRI_UNNECESSARYNODEREMOVERCOPIER_HPP

#include "indri/QuerySpec.hpp"
#include "indri/delete_range.hpp"

class UnnecessaryNodeRemoverCopier : public indri::lang::Copier {
protected:
  std::vector<indri::lang::Node*> _nodes;

  class SingleChildWalker : public indri::lang::Walker { 
  private:
    indri::lang::Node* _child;
    int _children;
    bool _seenRoot;

  public:
    SingleChildWalker() : _seenRoot(false), _children(0), _child(0) {}

    void defaultBefore( indri::lang::Node* n ) {
      if( !_seenRoot ) {
        _seenRoot = true;
      } else {
        _children++;
        _child = n;
      }
    }

    bool onlyOneChild() {
      return _children == 1;
    }

    indri::lang::Node* getChild() {
      return _child;
    }
  };

public:
  ~UnnecessaryNodeRemoverCopier() {
    delete_vector_contents( _nodes );
  }

  indri::lang::Node* defaultAfter( indri::lang::Node* old, indri::lang::Node* newNode ) {
    _nodes.push_back( newNode );
    return newNode;
  }

  indri::lang::Node* after( indri::lang::ExtentAnd* oldAnd, indri::lang::ExtentAnd* newAnd ) {
    SingleChildWalker walker;
    newAnd->walk(walker);

    if( walker.onlyOneChild() ) {
      delete newAnd;
      return walker.getChild();
    } else {
      _nodes.push_back( newAnd );
      return newAnd;
    }
  }

  indri::lang::Node* after( indri::lang::ExtentOr* oldOr, indri::lang::ExtentOr* newOr ) {
    SingleChildWalker walker;
    newOr->walk(walker);

    if( walker.onlyOneChild() ) {
      delete newOr;
      return walker.getChild();
    } else {
      _nodes.push_back( newOr );
      return newOr;
    }
  }

  indri::lang::Node* after( indri::lang::ODNode* oldOD, indri::lang::ODNode* newOD ) {
    SingleChildWalker walker;
    newOD->walk(walker);

    if( walker.onlyOneChild() ) {
      delete newOD;
      return walker.getChild();
    } else {
      _nodes.push_back( newOD );
      return newOD;
    }
  }

  indri::lang::Node* after( indri::lang::UWNode* oldUW, indri::lang::UWNode* newUW ) {
    SingleChildWalker walker;
    newUW->walk(walker);

    if( walker.onlyOneChild() ) {
      delete newUW;
      return walker.getChild();
    } else {
      _nodes.push_back( newUW );
      return newUW;
    }
  }
};

#endif // INDRI_UNNECESSARYNODEREMOVERCOPIER_HPP

