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
// AnchorTextWriter
//
// 20 May 2004 -- tds
//

#ifndef INDRI_ANCHORTEXTWRITER_HPP
#define INDRI_ANCHORTEXTWRITER_HPP

#include <iostream>
#include <algorithm>
#include "indri/Path.hpp"
#include "lemur-compat.hpp"

class AnchorTextWriter : public ObjectHandler<ParsedDocument> {
private:
  std::ofstream _out;

public:
  AnchorTextWriter( const std::string& outputPath ) {
    std::string directory = Path::directory( outputPath );
    Path::make( directory );
    _out.open( outputPath.c_str(), std::ios::out );
  }

  ~AnchorTextWriter() {
    _out.close();
  }

  void handle( ParsedDocument* document ) {
    greedy_vector<MetadataPair>::iterator iter;

    iter = std::find_if( document->metadata.begin(),
                         document->metadata.end(),
                         MetadataPair::key_equal( "DOCNO" ) );

    const char* docno = (char*)iter->value;

    iter = std::find_if( document->metadata.begin(),
                         document->metadata.end(),
                         MetadataPair::key_equal( "URL" ) );

    const char* page = (char*)iter->value;
    const char* url = 0;
    int count = 0;
    int urlEnd = -1;

    // find the third slash, which should occur
    // right after the domain name
    char* slash = 0;
    if(page)  slash = strchr( page, '/' );
    if(slash) slash = strchr( slash+1, '/' );
    if(slash) slash = strchr( slash+1, '/' );

    int domainLength;
    if( slash )
      domainLength = slash - page;
    else
      domainLength = strlen(page);

    // count links
    for( unsigned int i=0; i<document->tags.size(); i++ ) {
      TagExtent& extent = document->tags[i];

      // we only extract absolute urls
      if( !strcmp( extent.name, "absolute-url" ) ) {
        url = document->terms[ extent.begin ];
        urlEnd = extent.end;

        // if it has the same domain, throw it out
        if( url && page && !lemur_compat::strncasecmp( url, page, domainLength ) ) {
          url = 0;
          urlEnd = -1;
        }
      } else if( !strcmp( extent.name, "a" ) &&  // this is anchor text
                 url &&                          // we've seen a url
                 urlEnd == extent.begin &&       // this text is associated with an absolute-url
                 extent.end - extent.begin > 0 ) // there is some text here
      {
        count++;
        url = 0;
      }
    }

    // print output
    _out << "DOCNO=" << docno << std::endl;
    _out << "DOCURL=" << page << std::endl;
    _out << "LINKS=" << count << std::endl;
    url = 0;
    urlEnd = -1;

    for( unsigned int i=0; i<document->tags.size(); i++ ) {
      TagExtent& extent = document->tags[i];

      if( !strcmp( extent.name, "absolute-url" ) ) {  // this is an absolute url
        url = document->terms[ extent.begin ];
        urlEnd = extent.end;

        // if it has the same domain, throw it out
        if( url && page && !lemur_compat::strncasecmp( url, page, domainLength ) ) {
          url = 0;
          urlEnd = -1;
        }
      } else if( !strcmp( extent.name, "a" ) &&  // this is anchor text
                 url &&                          // we've seen a url
                 urlEnd == extent.begin &&       // this text is associated with an absolute-url
                 extent.end - extent.begin > 0 ) // there is some text here
      {
        int textLength = 0;

        _out << "LINKURL=" << url << std::endl;
        _out << "TEXT=\"";
        for( unsigned int j=extent.begin; j < extent.end && textLength < 60000; j++ ) {
          if( !document->terms[j] )
            continue;

          textLength += strlen(document->terms[j])+1;
          _out << document->terms[j] << " ";
        }
        _out << "\"" << std::endl;

        // only do the same link once
        url = 0;
      }
    }
  }
};

#endif // INDRI_ANCHORTEXTWRITER_HPP

