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
// AnchorTextAnnotator
//
// 25 May 2004 -- tds
//
// Reads anchor text in from files created by the 
// combiner, and adds the text to the end of the
// parsed document
//

#ifndef INDRI_ANCHORTEXTANNOTATOR_HPP
#define INDRI_ANCHORTEXTANNOTATOR_HPP

#include "indri/Buffer.hpp"
#include "indri/Transformation.hpp"
#include <iostream>
#include "indri/TagExtent.hpp"
#include "indri/ParsedDocument.hpp"
#include <fstream>

class AnchorTextAnnotator : public Transformation {
  std::ifstream _in;
  char _docno[256];
  int _count;
  Buffer _buffer;
  ObjectHandler<ParsedDocument>* _handler;

  void _readDocumentHeader() {
    char line[65536];

    if( !_in.good() || _in.eof() )
      return;

    // DOCNO=
    _in.getline( _docno, sizeof _docno-1 );
    // DOCURL=
    _in.getline( line, sizeof line-1 );

    // LINKS=
    _in.getline( line, sizeof line-1 );
    _count = atoi( line+6 );
  }

  void _fetchText( greedy_vector<TagExtent>& tags, greedy_vector<char*>& terms ) {
    // first, surround current text with a mainbody tag
    TagExtent mainbody;
    mainbody.begin = 0;
    mainbody.end = terms.size();
    mainbody.name = "mainbody";
    mainbody.number = 0;

    greedy_vector<TagExtent> oldTags;
    oldTags = tags;
    tags.clear();
    tags.push_back( mainbody );
    tags.append( oldTags.begin(), oldTags.end() );

    // now, fetch the additional terms
    char line[65536];
    _buffer.clear();

    for( int i=0; i<_count; i++ ) {
      // LINK
      _in.getline( line, sizeof line-1 );

      // TEXT=
      _in.getline( line, sizeof line-1 );
      int textLen = strlen(line+6);
      strcpy( _buffer.write(textLen+1), line+6 );
      _buffer.unwrite(1);

      assert( *(_buffer.front()+_buffer.position()-1) == '\"' && "Last character should be a quote" );
    }
    *(_buffer.write(1)) = 0;

    // now there's a bunch of text in _buffer, space separated, with each
    // link separated by a " symbol

    char* beginWord = 0;
    int beginIndex = 0;
    char* buffer = _buffer.front();

    for( unsigned int i=0; i<_buffer.position(); i++ ) {
      if( isalnum(buffer[i]) && !beginWord ) {
        beginWord = buffer+i;

        if(!beginIndex)
          beginIndex = terms.size();
      } else if( isspace(buffer[i]) ) {
        if( beginWord )
          terms.push_back( beginWord );
        buffer[i] = 0;
        beginWord = 0;
      } else if( buffer[i] == '\"' ) {
        buffer[i] = 0;
        if( beginWord )
          terms.push_back( beginWord );
        beginWord = 0;
        
        TagExtent extent;
        extent.name = "inlink";
        extent.begin = beginIndex;
        extent.end = terms.size();
        extent.number = 0;

        assert( extent.begin <= extent.end );

        if( beginIndex )
          tags.push_back(extent);

        beginIndex = 0;
      }
    }
  }

  bool _matchingDocno( ParsedDocument* document ) {
    // find DOCNO attribute in document
    for( size_t i=0; i<document->metadata.size(); i++ ) {
      const char* attributeName = document->metadata[i].key;
      const char* attributeValue = (const char*) document->metadata[i].value;

      if( !strcmp( attributeName, "docno" ) ) {
        if( !strcmp( attributeValue, _docno+6 ) ) {
          return true;
        } else {
          return false;
        }
      }
    }
 
    return false;
  }

public:
  AnchorTextAnnotator() {
    _handler = 0;
  }

  ~AnchorTextAnnotator() {
    _in.close();
  }

  void open( const std::string& anchorFile ) {
    _in.close();
    _in.clear();
    _in.open( anchorFile.c_str() );
    _buffer.clear();
    _readDocumentHeader();
  }

  ParsedDocument* transform( ParsedDocument* document ) {
    if( _matchingDocno( document ) ) {
      _fetchText( document->tags, document->terms );
      _readDocumentHeader();
    }
  
    return document;
  }

  void setHandler( ObjectHandler<ParsedDocument>& handler ) {
    _handler = &handler;
  }

  void handle( ParsedDocument* document ) {
    _handler->handle( transform( document ) );
  }
};

#endif // INDRI_ANCHORTEXTANNOTATOR_HPP

