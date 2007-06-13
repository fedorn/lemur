/*==========================================================================
 * Copyright (c) 2003-2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
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
#include <algorithm>

/*! Top level namespace for all indri components. */
namespace indri
{
  /*! \brief File input, parsing, stemming, and stopping classes. */
  namespace parse
  {
    /*! Reads anchor text in from files created by the 
      combiner, and adds the text to the end of the parsed document
    */    
    class AnchorTextAnnotator : public Transformation {
      std::ifstream _in;
      char _docno[256];
      int _count;
      indri::utility::Buffer _buffer;
      ObjectHandler<indri::api::ParsedDocument>* _handler;

      void _readDocumentHeader() {
        char line[65536];
        _count = 0;

        if( !_in.good() || _in.eof() )
          return;

        // DOCNO=
        _in.getline( _docno, sizeof _docno-1 );
        if( !_in.good() || _in.eof() )
          return;

        // DOCURL=
        _in.getline( line, sizeof line-1 );
        if( !_in.good() || _in.eof() )
          return;

        // LINKS=
        _in.getline( line, sizeof line-1 );

        _count = atoi( line+6 );
      }

      void _fetchText( indri::utility::greedy_vector<TagExtent *>& tags, indri::utility::greedy_vector<char*>& terms ) {
        // now, fetch the additional terms
        char line[65536];
        _buffer.clear();
        for( int i=0; i<_count; i++ ) {
          // LINK
          _in.getline( line, sizeof line-1 );

          // LINKDOCNO 
          _in.getline( line, sizeof line-1 );
          
          // TEXT=
          _in.getline( line, sizeof line-1 );
          if (!line[0]) continue;
          size_t textLen = strlen(line+6);
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
#ifndef WIN32
          if( isalnum(buffer[i]) && !beginWord ) {
#else
          if( (buffer[i] >= 0 && isalnum(buffer[i])) && !beginWord ) {
#endif
            beginWord = buffer+i;

            if(!beginIndex)
              beginIndex = (int)terms.size();
          } else if( isspace(buffer[i]) ) {
            buffer[i] = 0;
            if( beginWord )
              terms.push_back( beginWord );
            beginWord = 0;
          } else if( buffer[i] == '\"' ) {
            buffer[i] = 0;
            if( beginWord )
              terms.push_back( beginWord );
            beginWord = 0;

            if( beginIndex ) {        
              TagExtent * extent = new TagExtent;
              extent->name = "inlink";
              extent->begin = beginIndex;
              extent->end = (int)terms.size();
              extent->number = 0;
              extent->parent = 0;

              assert( extent->begin <= extent->end );

              tags.push_back(extent);
              if( terms.size() > 125000 )
                break;
            }


            beginIndex = 0;
          }

        }

      }

      bool _matchingDocno( indri::api::ParsedDocument* document ) {
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

      indri::api::ParsedDocument* transform( indri::api::ParsedDocument* document ) {
        // surround current text with a mainbody tag
        TagExtent * mainbody = new TagExtent;
        mainbody->begin = 0;
        mainbody->end = (int)document->terms.size();
        mainbody->name = "mainbody";
        mainbody->number = 0;
        mainbody->parent = 0;
        // order no longer matters - the indexer takes care of any sorting needed
        document->tags.push_back( mainbody );

        // set old tags' parent to mainbody
        indri::utility::greedy_vector<TagExtent *>::iterator oldTag = document->tags.begin();
        indri::utility::greedy_vector<TagExtent *>::iterator oldTagsEnd = document->tags.end();
        while ( oldTag != oldTagsEnd ) {
          if ((*oldTag)->parent == 0) {
            (*oldTag)->parent = mainbody;
          }
          oldTag++;
        }

        if( _matchingDocno( document ) ) {
          _fetchText( document->tags, document->terms );
          _readDocumentHeader();
        }
        std::sort( document->tags.begin(), document->tags.end(), indri::parse::LessTagExtent() );
        return document;
      }

      void setHandler( ObjectHandler<indri::api::ParsedDocument>& handler ) {
        _handler = &handler;
      }

      void handle( indri::api::ParsedDocument* document ) {
        _handler->handle( transform( document ) );
      }
    };
  }
}

#endif // INDRI_ANCHORTEXTANNOTATOR_HPP

