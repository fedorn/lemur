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
// Unpacker
//
// 22 March 2004 -- tds
//

#include "indri/Unpacker.hpp"
#include "indri/QuerySpec.hpp"
#include "indri/XMLNode.hpp"
#include "Exception.hpp"

namespace indri {
  namespace lang {
    // Note: _unpack is automatically generated in UnpackerInternal.cpp

    Unpacker::Unpacker( XMLNode* root ) :
      _root(root)
    {
    }

    std::vector<indri::lang::Node*> Unpacker::unpack() {
      std::vector<indri::lang::Node*> result;

      for( unsigned int i=0; i<_root->getChildren().size(); i++ ) {
        XMLNode* child = _root->getChildren()[i];
        _current = child;
        Node* node = _unpack(child);
        _nodes[node->nodeName()] = node;

        if( child->getAttribute( "root" ) == "true" ) {
          result.push_back(node);
        } 
      }

      return result;
    }

    ::std::string Unpacker::getString( const char* stringName ) {
      assert( _current );
      return _current->getAttribute( stringName );
    }

    double Unpacker::getDouble( const char* name ) {
      std::string value = _current->getAttribute( name );
      std::stringstream s(value);
      double v;
      s >> v;
      return v;
    }

    UINT64 Unpacker::getInteger( const char* name ) {
      std::string value = _current->getAttribute( name );
      return string_to_i64( value );
    }

    RawExtentNode* Unpacker::getRawExtentNode( const char* name ) {
      std::string internalName = _current->getChildValue(name);
      return (RawExtentNode*) _nodes[internalName];
    }

    ScoredExtentNode* Unpacker::getScoredExtentNode( const char* name ) {
      std::string internalName = _current->getChildValue(name);
      return (ScoredExtentNode*) _nodes[internalName];
    } 

    ::std::vector<RawExtentNode*> Unpacker::getRawExtentVector( const char* name ) {
      return getNodeVector<RawExtentNode>(name);
    }

    ::std::vector<ScoredExtentNode*> Unpacker::getScoredExtentVector( const char* name ) {
      return getNodeVector<ScoredExtentNode>(name);
    }

    ::std::vector<int> Unpacker::getIntVector( const char* name ) {
      std::vector<int> result;
      const XMLNode* vector = _current->getChild(name);

      for( unsigned int i=0; i<vector->getChildren().size(); i++ ) {
        XMLNode* ref = vector->getChildren()[i];
        std::stringstream s( ref->getValue() );
        int value;
        s >> value;
        result.push_back( value );
      }

      return result;
    }

    ::std::vector<double> Unpacker::getDoubleVector( const char* name ) {
      std::vector<double> result;
      const XMLNode* vector = _current->getChild(name);

      for( unsigned int i=0; i<vector->getChildren().size(); i++ ) {
        XMLNode* ref = vector->getChildren()[i];
        std::stringstream s( ref->getValue() );
        double value;
        s >> value;
        result.push_back( value );
      }

      return result;
    }

    bool Unpacker::getBoolean( const char* name ) {
      return (getInteger(name) ? true : false );
    }  
  }
}


