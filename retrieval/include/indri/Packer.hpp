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
// Packer
//
// 17 March 2004 -- tds
//

#ifndef INDRI_PACKER_HPP
#define INDRI_PACKER_HPP

#include "indri/XMLNode.hpp"
#include <stack>
#include <vector>
#include <string>

namespace indri {
  namespace lang {
    class RawExtentNode;
    class ScoredExtentNode;

    class Packer {
    private:
      struct node_element {
        // be warned--there is no defined copy constructor
        // here and no destructor; memory management must be
        // done by the user
        node_element() : languageNode(0), xmlNode(0), flushed(false) {}
        node_element( class Node* langNode ) :
          languageNode( langNode ),
          xmlNode( new XMLNode( "node" ) ),
          flushed(false)
        {
        }

        class Node* languageNode;
        XMLNode* xmlNode;
        bool flushed;
      };

      std::map<class Node*, node_element*> _elements;
      std::stack<node_element*> _stack;
      XMLNode* _packedNodes;

      node_element* _getElement( class Node* node );
      XMLNode* _getNodeReference( class Node* node, const std::string& name );

    public:
      Packer();

      void before( class Node* someNode );
      void after( class Node* someNode );
      void put( const char* name, bool value );
      void put( const char* name, int value );
      void put( const char* name, unsigned int value );
      void put( const char* name, UINT64 value );
      void put( const char* name, double value );
      void put( const char* name, const std::string& value );
      void put( const char* name, const std::vector<int>& value );
      void put( const char* name, const std::vector<double>& value );
      void put( const char* name, const std::vector<RawExtentNode*>& value );
      void put( const char* name, const std::vector<ScoredExtentNode*>& value );
      void put( const char* name, Node* value );
      void pack( class indri::lang::Node* root );
      void pack( std::vector<class indri::lang::Node*>& roots );
      std::string toString();
      XMLNode* xml();
    };
  }
}

#endif // INDRI_PACKER_HPP
