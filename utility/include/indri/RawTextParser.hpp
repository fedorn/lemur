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
// RawTextParser
//
// 10 February 2004 -- tds
//

#ifndef INDRI_RAWTEXTPARSER_HPP
#define INDRI_RAWTEXTPARSER_HPP

#define PARSER_MAX_WORD_LENGTH (30)

class RawTextParser {
private:
  std::ifstream _in;
  char* _buffer;
  char* _current;
  int _bufferSize;

public:
  RawTextParser( int memorySize = 1024*1024 ) {
    _bufferSize = memorySize;
    _buffer = new char[_bufferSize]; // let's hope that 1mb is enough for any docs we're gonna see
  }

  ~RawTextParser() {
    delete _buffer;
  }

  bool open( const std::string& fileName ) {
    _in.open( fileName.c_str(), std::ifstream::in );
    return _in.good();
  }

  void close() {
    _in.close();
  }

  /// parses the next document in the filestream, returning a list of char* to words
  /// the words are stored in a character buffer within the parser, which means that
  /// if you call parseDocument again, all your old word pointers are invalid.

  bool parseDocument( std::string& docName, greedy_vector<char*>& words ) { 
    static const char docPrefix[] = "<DOC>";
    static const char endDocPrefix[] = "</DOC>";
    static const char docnoPrefix[] = "<DOCNO ";
    static const char urlPrefix[] = "<URL";
    bool gotDocID = false;
    int bufferPos = 0;

    while( 1 ) {
      int remainingSpace = _bufferSize - bufferPos;
      _in.getline( _buffer + bufferPos, remainingSpace );
      int length = _in.gcount();

      if( _in.rdstate() & (std::ifstream::failbit|std::ifstream::eofbit) ) {
        if( _in.rdstate() & std::ifstream::eofbit ) {
          return false; // at end of file, we're done
        }
        
        if( length == remainingSpace - 1 ) {
          throw Exception( "RawTextParser", "Buffer size is too small to handle some document in the corpus, use -parserMemory to change." );
        }

        if( _in.rdstate() & std::ifstream::failbit ) {
          throw Exception( "RawTextParser", "Unable to recover from failed read" );
        }
      }

      _buffer[bufferPos+length] = 0;
      char* line = _buffer + bufferPos;

      if( length && _buffer[bufferPos] == '<' ) {
        if( length > sizeof docnoPrefix-1 && !strncmp( docnoPrefix, line, sizeof docnoPrefix-1 ) ) {
          docName.assign( line+sizeof docnoPrefix-1, line + length - 2 );
          gotDocID = true;
        } else if ( length > sizeof endDocPrefix-1 && !strncmp( endDocPrefix, line, sizeof endDocPrefix-1 ) ) {
          // handle end doc -- return
          if( gotDocID )
            return true;
        }
      } else {
        if( !gotDocID )
          continue;

        int i = 0;

        while(1) {
          for( ; isspace(line[i]) && i<length && line[i]; i++ )
            ;

          if( i>= length || !line[i] )
            break;

          char* begin = &line[i];

          for( ; !isspace(line[i]) && i<length && line[i]; i++ )
            ;

          line[i] = 0;
          i++;

          if( &line[i] - begin > PARSER_MAX_WORD_LENGTH )
            begin[PARSER_MAX_WORD_LENGTH-1] = 0;

          words.push_back(begin);
        }
      }

      bufferPos += length + 1; // have to skip trailing \0
    }
  }
};

#endif // INDRI_RAWTEXTPARSER_HPP
