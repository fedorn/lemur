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
// XMLReader
//
// 8 October 2003 - tds
//

#include "indri/XMLReader.hpp"
#include "Exception.hpp"

int XMLReader::_tryFindChar( char ch, const char* buffer, int start, int finish ) {
  int i;

  for( i=start; i<finish; i++ ) {
    if( buffer[i] == ch )
      break;
  }

  if( i==finish )
    return -1;

  return i;
}

int XMLReader::_findChar( char ch, const char* buffer, int start, int finish ) {
  int result = _tryFindChar( ch, buffer, start, finish );

  if( result == -1 )
    LEMUR_THROW( LEMUR_PARSE_ERROR, "Was looking for '" + ch + "', but couldn't find it." );

  return result;
}

int XMLReader::_tryFindBeginTag( const char* buffer, int start, int finish ) {
  return _tryFindChar( '<', buffer, start, finish );
}

int XMLReader::_findBeginTag( const char* buffer, int start, int finish ) {
  int result = _tryFindBeginTag( buffer, start, finish );

  if( result == -1 )
    LEMUR_THROW( LEMUR_PARSE_ERROR, "Ran off the end of a buffer while looking for a begin tag" );

  return result;
}

int XMLReader::_findEndTag( const char* buffer, int start, int finish ) {
  return _findChar( '>', buffer, start, finish );
}

int XMLReader::_tryFindText( const char* buffer, int start, int finish ) {
  int i;

  for( i=start; i<finish; i++ ) {
    if( !isspace(buffer[i]) )
      break;
  }

  return i;
}

int XMLReader::_findText( const char* buffer, int start, int finish ) {
  int result = _tryFindText( buffer, start, finish );
  if( result==finish )
    LEMUR_THROW( LEMUR_GENERIC_ERROR, "Was looking for text, but couldn't find any" );

  return result;
}

int XMLReader::_findNotName( const char* buffer, int start, int finish ) {
  int i;

  for( i=start; i<finish; i++ ) {
    // this isn't unicode-safe, but it should be good for now
    if( !isalpha(buffer[i]) && 
      !isdigit(buffer[i]) &&
      buffer[i] != '-' &&
      buffer[i] != '_' &&
      buffer[i] != ':' &&
      buffer[i] != '.' ) {
        break;
      }
  }

  if( i==finish )
    LEMUR_THROW( LEMUR_PARSE_ERROR, "Was looking for the end of a tag name, but couldn't find it." );

  return i;
}

int _findSpace( char* buffer, int start, int finish ) {
  int i;

  for( i=start; i<finish; i++ ) {
    if( isspace(buffer[i]) )
      break;
  }

  if( i==finish )
    LEMUR_THROW( LEMUR_PARSE_ERROR, "Was looking for a space, but couldn't find it." );

  return i;
}

int XMLReader::_readTag( const char* buffer, int bufferStart, int bufferEnd, std::string* tagName, std::map<std::string, std::string>* attributes, int* tagType ) {
  // skip opening whitespace
  int startLocation = bufferStart;
  int endLocation = _findEndTag( buffer, startLocation, bufferEnd );
  int position = startLocation+1;
  int trueEndLocation = endLocation+1;

  if( endLocation - position < 1 ) 
    LEMUR_THROW( LEMUR_GENERIC_ERROR, "Found a tag with no body" );

  // is it an opening tag?
  if( buffer[position] == '/' ) {
    if( tagType )
      *tagType = TAG_CLOSE_TYPE;
    position++;

    if( position >= endLocation )
      LEMUR_THROW( LEMUR_GENERIC_ERROR, "Found a tag with no body" );
  } else {
    if( buffer[endLocation-1] == '/' ) {
      if( tagType )
        *tagType = TAG_OPEN_CLOSE_TYPE;
      endLocation--;
    } else if( tagType ) {
      *tagType = TAG_OPEN_TYPE;
    }
  }

  if( tagName || attributes ) {
    int textBegin = _findText( buffer, position, endLocation );
    int textEnd = _findNotName( buffer, textBegin, endLocation+1 );

    if( tagName )
      tagName->assign( &buffer[textBegin], &buffer[textEnd] );

    position = textEnd;

    if( attributes ) {
      attributes->clear();

      textBegin = _findText( buffer, position, endLocation+1 );
      position = textBegin;

      for( ; position != endLocation; position = _tryFindText( buffer, position, endLocation ) ) {
        textEnd = _findNotName( buffer, position, endLocation );
        int equalsPosition = _findChar( '=', buffer, textEnd, endLocation );
        int quotePosition = _findText( buffer, equalsPosition+1, endLocation );
        int endQuotePosition = _findChar( buffer[quotePosition], buffer, quotePosition+1, endLocation );

        std::string attributeName;
        std::string valueText;

        assert( position <= textEnd );
        assert( quotePosition+1 <= endQuotePosition );
        assert( textEnd < quotePosition+1 );

        attributeName.assign( &buffer[position], &buffer[textEnd] );
        valueText.assign( &buffer[quotePosition+1], &buffer[endQuotePosition] );

        attributes->insert( std::make_pair( attributeName, valueText ) );
        position = endQuotePosition+1;
      }
    }
  }

  return trueEndLocation;
}

int XMLReader::_findClosingTag( const char* buffer, int start, int finish, std::string& openingTagName, bool* tagsBetween ) {
  int openingTags = 0;
  int closingTags = 0;
  int position = start;
  bool done = false;
  bool match = false;
  int tagType;

  if( tagsBetween )
    *tagsBetween = false;
  try {
    while( !done ) {
      std::string tagName;
      position = _findBeginTag( buffer, position, finish );
      int end = _readTag( buffer, position, finish, &tagName, NULL, &tagType );

      if( tagType != TAG_CLOSE_TYPE ) {
        if( tagsBetween )
          *tagsBetween = true;

        if( tagType == TAG_OPEN_TYPE )
          openingTags++;
        position = end;

        while( openingTags > closingTags ) {
          // don't need to check for matching tags here, we just need to
          // count open and closed tags
          position = _findBeginTag( buffer, position, finish );
          end = _readTag( buffer, position, finish, NULL, NULL, &tagType );
          position = end;

          if( tagType == TAG_OPEN_TYPE ) {
            openingTags++;
          } else if( tagType == TAG_CLOSE_TYPE ) {
            closingTags++;
          }
        }
      } else {
        match = (tagName == openingTagName);
        done = true;
      }
    }
  } catch( Exception& e ) {
    LEMUR_RETHROW( e, std::string() + "Caught an error while looking for an end tag for '" + openingTagName + "'" );
  }

  if( match ) {
    return position;
  } else {
    return -1;
  }
}

void XMLReader::_read( XMLNode** parent, const char* buffer, int start, int end ) {
  int tagType;

  for( int current = _tryFindBeginTag( buffer, start, end );
    current >= 0;
    current = _tryFindBeginTag( buffer, current, end ) ) {
      XMLNode* node;
      std::string tagName;
      std::map<std::string, std::string> attributes;
      bool tagsBetween;

      int endLevel;
      int endTag = _readTag( buffer, current, end, &tagName, &attributes, &tagType );

      if( tagType == TAG_CLOSE_TYPE )
        LEMUR_THROW( LEMUR_GENERIC_ERROR, "Found a close tag for '" + tagName + "' while looking for an open tag." );

      if( tagType == TAG_OPEN_TYPE ) {
        int closingTag = _findClosingTag( buffer, endTag, end, tagName, &tagsBetween );

        if( tagsBetween ) {
          node = new XMLNode( tagName, attributes );
          _read( &node, buffer, endTag, closingTag );
        } else {
          std::string nodeValue;
          nodeValue.assign( &buffer[endTag], &buffer[closingTag] ); 
          node = new XMLNode( tagName, attributes, nodeValue );
        }

        endLevel = _findEndTag( buffer, closingTag, end )+1;
      } else {
        assert( tagType == TAG_OPEN_CLOSE_TYPE );
        node = new XMLNode( tagName, attributes );
        endLevel = endTag;
      }

      if( *parent ) {
        (*parent)->addChild( node );
      } else {
        *parent = node;
        break;
      }

      current = endLevel;
    }
}

XMLNode* XMLReader::read( const char* buffer, size_t length ) {
  XMLNode* result = NULL;
  _read( &result, buffer, 0, int(length) );
  return result;
}

XMLNode* XMLReader::read( const std::string& str ) {
  return read( str.c_str(), str.length() );
}


