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
// XMLNode
//
// 8 October 2003 - tds
//

#include "indri/XMLNode.hpp"

XMLNode::XMLNode( const std::string& name ) : _name(name)
{
}

XMLNode::XMLNode( const std::string& name, const std::string& value ) : _name(name), _value(value)
{
}

XMLNode::XMLNode( const std::string& name, const MAttributes& attributes ) : _name(name), _attributes(attributes)
{
}

XMLNode::XMLNode( const std::string& name, const MAttributes& attributes, const std::string& value ) : _name(name), _attributes(attributes), _value(value)
{
}

XMLNode::~XMLNode() {
  for( unsigned int i=0; i<_children.size(); i++ )
    delete _children[i];
}

void XMLNode::addChild( XMLNode* child ) {
  _children.push_back(child);
}

void XMLNode::addAttribute( const std::string& key, const std::string& value ) {
  _attributes.insert( std::make_pair( key, value ) );
}

const std::string& XMLNode::getName() const {
  return _name;
}

const std::string& XMLNode::getValue() const {
  return _value;
}

const XMLNode::MAttributes& XMLNode::getAttributes() const {
  return _attributes;
}

std::string XMLNode::getAttribute( const std::string& name ) const {
  MAttributes::const_iterator iter;
  iter = _attributes.find( name );

  if( iter != _attributes.end() ) {
    return iter->second;
  }

  return std::string();
}

const std::vector<XMLNode*>& XMLNode::getChildren() const {
  return _children;
}

const XMLNode* XMLNode::getChild( const std::string& name ) const {
  for( unsigned int i=0; i<_children.size(); i++ ) {
    if( _children[i]->getName() == name )
      return _children[i];
  }

  return 0;
}

std::string XMLNode::getChildValue( const std::string& name ) const {
  const XMLNode* child = getChild(name);

  if( child ) {
    return child->getValue();
  }

  return std::string();
}

void XMLNode::setValue( const std::string& value ) {
  _value = value;
}


