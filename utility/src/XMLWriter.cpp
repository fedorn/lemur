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
// XMLWriter
//
// 8 October 2003 - tds
//

#include "indri/XMLWriter.hpp"

void XMLWriter::_writeChar( char ch, std::string& output ) const {
  output += ch;
}

void XMLWriter::_writeTabs( int tabs, std::string& output ) const {
  for( int i=0; i<tabs; i++ ) {
    _writeChar( '\t', output );
  }
}

void XMLWriter::_writeTag( const std::string& tag, const XMLNode::MAttributes& attributes, std::string& output, bool opening ) const {
  _writeChar( '<', output );

  if( !opening ) {
    _writeChar( '/', output );
  }
  output += tag;

  if( opening ) {
    XMLNode::MAttributes::const_iterator iter;
    for( iter = attributes.begin(); iter != attributes.end(); iter++ ) {
      _writeChar( ' ', output );
      output += iter->first;

      _writeChar( '=', output );
      _writeChar( '\"', output );
      output += iter->second;
      _writeChar( '\"', output );
    }
  }

  _writeChar( '>', output );
}

void XMLWriter::_writeEndOfLine( std::string& output ) const {
  _writeChar( '\n', output );
}

void XMLWriter::_writeXML( int tabs, const XMLNode* node, std::string& output ) const {
  // write opening tag
  _writeTabs( tabs, output );
  _writeTag( node->getName(), node->getAttributes(), output, true );

  if( node->getChildren().size() ) {
    _writeEndOfLine( output );

    for( unsigned int i=0; i<node->getChildren().size(); i++ ) {
      _writeXML( tabs+1, (node->getChildren())[i], output );
    }

    _writeTabs( tabs, output );
  } else {
    output += node->getValue();
  }

  _writeTag( node->getName(), node->getAttributes(), output, false );
  _writeEndOfLine( output );
}

XMLWriter::XMLWriter( XMLNode* node ) : _node(node)
{
}

void XMLWriter::write( std::string& output ) {
  _writeXML( 0, _node, output );
}

