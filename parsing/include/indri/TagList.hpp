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
// TagList
//
// March 2004 -- metzler
//

#include "indri/Tag.hpp"
#include <stdio.h>
#include <string.h>
#include <indri/greedy_vector>
#include "indri/TagExtent.hpp"
#include <iostream>
#include "indri/MetadataPair.hpp"
#include "indri/Buffer.hpp"

#ifndef _TAGLIST_HPP
#define _TAGLIST_HPP

class TagList {
private:
  struct tag_entry {
    const char* name;
    const char* conflation;
    int next;
    int begin;
    int end;
  };

  greedy_vector<tag_entry> _tags;
  int _openList;

public:
  TagList() {
    clear();
  }

  void clear() {
    _tags.clear();
    _openList = -1;
  }

  // we assume here that name is more or less immutable
  // so we can store a pointer to it.  This is a reasonable
  // assumption, because if the tag is indexed, its name is
  // in a hash table somewhere, and we can just point to that
  // name copy.
  void addTag(const char *name, const char* conflation, int begin) {
    // because of conflations, all kinds of messy stuff
    // happens if there's already an open tag with the same
    // conflation as this one.  Therefore, we have to go looking
    // for all open tags with this conflation; if there are any,
    // this tag doesn't get added.
    int list = _openList;

    while( list >= 0 ) {
      tag_entry& entry = _tags[list];

      if( !strcmp( entry.conflation, conflation ) ) {
        // we already have one of these
        return;
      }
   
      list = entry.next;
    }

    // all clear now to add the tag:
    tag_entry t;
    t.name = name;
    t.conflation = conflation;
    t.begin = begin;
    t.end = -1;
    t.next = _openList;
    _tags.push_back(t);
    _openList = _tags.size()-1;
  }

  void endTag(const char *name, const char* conflation, int end) {
    int list = _openList;
    int prev = -1;

    while( list >= 0 ) {
      tag_entry& entry = _tags[list];

      if( !strcmp( entry.name, name ) ) {
        // found a tag to close
        entry.end = end;
        int next = entry.next;

        // unlink from open list
        if( prev == -1 ) {
          _openList = next;
        } else {
          _tags[prev].next = next;
        }
        
        return;
      } else {
        // this wasn't the tag, so keep looking
        prev = list;
        list = entry.next;
      }
    }
  }

  void writeTagList( greedy_vector<TagExtent>& tags ) {
    // look through the tags vector; they're already in sorted order by open
    // position.  Only add closed tags.

    for( unsigned int i=0; i<_tags.size(); i++ ) {
      tag_entry& entry = _tags[i];

      if( entry.end > 0 ) {
        TagExtent extent;
        extent.begin = entry.begin;
        extent.end = entry.end;
        extent.name = entry.conflation;
        extent.number = 0;
        tags.push_back(extent);
      }
    }
  }

  // in this case, we'll treat the list of tags in this list
  // as if they were offsets into a metadata list
  void writeMetadataList( greedy_vector<MetadataPair>& pairs, Buffer& buffer, const char* docText ) {
    for( unsigned int i=0; i<_tags.size(); i++ ) {
      tag_entry& entry = _tags[i];

      if( entry.end > 0 ) {
        MetadataPair pair;
        
        // copy the text into a buffer
        int length = entry.end - entry.begin;
        char* spot = buffer.write(length+1);
        strncpy( spot, docText + entry.begin, length);
        spot[length] = 0;

        pair.key = entry.conflation;
        pair.value = spot;
        pair.valueLength = length+1;

        // docno is special -- its value must be stripped
        if( !strcmp( pair.key, "docno" ) ) {
          pair.stripValue();
        }

        pairs.push_back(pair);
      }
    }
  }

};

#endif
