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


