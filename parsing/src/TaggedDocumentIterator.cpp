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
// TaggedDocumentIterator
//
// 14 May 2004 -- tds
//

#include "indri/TaggedDocumentIterator.hpp"
#include <iostream>
#include "Exception.hpp"
#include "indri/XMLNode.hpp"

struct metadata_range {
  int beginText;
  int endText;
  int beginTag;
  int endTag;
};

TaggedDocumentIterator::TaggedDocumentIterator() {
  _in = 0;

  _startDocTag = 0;
  _endDocTag = 0;
  _endMetadataTag = 0;

  _startDocTagLength = 0;
  _endDocTagLength = 0;
  _endMetadataTagLength = 0;
}

TaggedDocumentIterator::~TaggedDocumentIterator() {
  close();
}

void TaggedDocumentIterator::setTags( const char* startDoc, const char* endDoc, const char* endMetadata ) {
  _startDocTag = startDoc;
  _startDocTagLength = strlen(startDoc);
  _endDocTag = endDoc;
  _endDocTagLength = strlen(endDoc);
  _endMetadataTag = endMetadata;
  _endMetadataTagLength = endMetadata ? strlen(endMetadata) : 0;
}

void TaggedDocumentIterator::open( const std::string& filename ) {
  _fileName = filename;
  _in = fopen( filename.c_str(), "r" );

  if( !_in )
    LEMUR_THROW( LEMUR_IO_ERROR, "Couldn't open file " + filename + "." );
}

void TaggedDocumentIterator::close() {
  if( _in )
    fclose( _in );
  _in = 0;
}

bool TaggedDocumentIterator::_readLine( char*& beginLine, size_t& lineLength ) {
  lineLength = 0;
  size_t actual;

  // make a buffer of a reasonable size so we're not always allocating
  if( _buffer.size() < 1024*1024 )
    _buffer.grow( 1024*1024 );

  if( _buffer.size()/2 < _buffer.position() )
    _buffer.grow( 2*_buffer.size() );

  if( _buffer.position() ) {
    // get rid of null terminator from previous call
    _buffer.unwrite(1);
  }

  size_t readAmount = _buffer.size() - _buffer.position() - 2;

  // fetch next document line
  char* buffer = _buffer.write( readAmount );
  char* result = fgets( buffer, readAmount, _in );
 
  if(!result) {
    return false;
  }

  actual = strlen(buffer);
  lineLength += actual; 
  _buffer.unwrite( readAmount - actual );

  // all finished reading
  *_buffer.write(1) = 0;
  beginLine = _buffer.front() + _buffer.position() - lineLength - 1;

  return true;
}

UnparsedDocument* TaggedDocumentIterator::nextDocument() {
  _document.metadata.clear();
  _buffer.clear();

  char* beginLine;
  size_t lineLength;  
  bool result;

  greedy_vector<metadata_range> metadata;

  // look for <DOC> tag
  do {
    result = _readLine( beginLine, lineLength );
  } while( result && strncmp( _startDocTag, beginLine, _startDocTagLength ) );
  
  if( !result ) {
    // didn't find a begin tag, so we're done
    return 0;
  }

  // read metadata tags
  bool openTag = false;
  int doclines = 0;

  if( _endMetadataTagLength > 0 ) {
    while( true ) {
      result = _readLine( beginLine, lineLength );

      if( !result ) {
        return 0;
      }

      //
      // the beginning of the line is either:
      //    an open tag
      //    text
      //    a close tag
      //
      if( *beginLine == '<' ) {
        if( openTag ) {
          // we've already seen an open tag, so we're only interesting in
          // a matching close tag
          const char* openTagText = metadata.back().beginTag + _buffer.front();
          int openTagLength = metadata.back().endTag - metadata.back().beginTag;
          
          if( beginLine[1] == '/' && !strncmp( openTagText, beginLine+2, openTagLength ) ) {
            // this is a close tag and it matches the open tag,
            // so tie up loose ends in the previous open tag metadata structure
            *beginLine = 0;
            metadata.back().endText = _buffer.position() - lineLength;
            openTag = false;
          }
        } else {
          // this might be a open-only tag, or there might be
          // a close tag at the end.
          char* endTag = strchr( beginLine, '>' );
          if( endTag ) {
            *endTag = 0;

            metadata_range range;
            range.beginTag = beginLine + 1 - _buffer.front();
            range.endTag = endTag - _buffer.front();

            char* endText = strchr( endTag + 1, '<' );

            if( (endTag - beginLine) < lineLength/2 && endText ) {
              // the end tag is also on this line
              range.beginText = endTag + 1 - _buffer.front();
              range.endText = endText - _buffer.front();
              *endText = 0;
              openTag = false;
            } else {
              // no more text on this line
              openTag = true;
              range.beginText = _buffer.position() - 1;
            }

            metadata.push_back( range );
          }
        }
      }

      if( !strncmp( _endMetadataTag + 1, beginLine + 1, _endMetadataTagLength - 2 ) ) {
        // all finished
        // have to skip the first letter because it may have been turned into a null

        if( openTag ) {
          // some tag was left open, so remove it
          metadata.erase( metadata.end()-1 );
        }
        break;
      }
    }
  }

  // from now on, everything is text
  int startDocument = _buffer.position() - 1;
  
  while(true) {
    result = _readLine( beginLine, lineLength );
    
    if( !result ) {
      LEMUR_THROW( LEMUR_IO_ERROR, "Malformed document: " + _fileName );
    }

    if( lineLength >= _endDocTagLength &&
        !strncmp( beginLine+lineLength-_endDocTagLength, _endDocTag, _endDocTagLength - 1 ) ) {
      beginLine[lineLength-_endDocTagLength] = 0;
      _document.text = _buffer.front() + startDocument;
      _document.textLength = _buffer.position() - startDocument - _endDocTagLength;
      // terminate document
      break;
    }
  }

  // parse metadata
  for( size_t i=0; i<metadata.size(); i++ ) {
    MetadataPair pair;
    char* key = _buffer.front() + metadata[i].beginTag;
    
    pair.key = _buffer.front() + metadata[i].beginTag;
    pair.value = _buffer.front() + metadata[i].beginText;
    pair.valueLength = metadata[i].endText - metadata[i].beginText + 1;

    // convert metadata key to lowercase
    for( char* c = key; *c; c++ ) {
      if( *c >= 'A' && *c <= 'Z' ) {
        *c += ('a' - 'A');
      }
    }

    // special handling for docno -- remove value spaces
    if( !strcmp( pair.key, "docno" ) ) {
      pair.stripValue();
    }

    _document.metadata.push_back(pair);
  }
  
  return &_document;
}

