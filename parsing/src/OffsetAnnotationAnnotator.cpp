/*==========================================================================
 * Copyright (c) 2003-2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// OffsetAnnotationAnnotator
//
// 18 September 2005 -- mwb
//
// Reads a supplied offset annotation file and adds the annotations to
// the parsed document.
//

// Format of the offset annotation file: 8-column, tab-delimited.
// From left-to-right, those columns are: 
//
//   docno     : external document id corresponding to the document in
//             : which the annotation occurs.
//
//   type      : TAG or ATTRIBUTE
//
//   id        : an id number for the annotation; each line should have a
//             : unique id >= 1.
//
//   name      : for TAG, name or type of the annotation
//             : for ATTRIBUTE, the attribute name, or key
//
//   start     : start and length define the annotation's extent;
//   length    : meaningless for an ATTRIBUTE
//
//   value     : for TAG, an INT64
//             : for ATTRIBUTE, a string that is the attribute's value
//
//   parentid  : for TAG, refers to the id number of another TAG to be
//             : considered the parent of this one; this is how hierarchical
//             : annotations can be expressed.
//             : a TAG that has no parent has parentid = 0
//             : for ATTRIBUTE, refers to the id number of a TAG to which
//             : it belongs and from which it inherits its start and length.
//             : *NOTE: the file must be sorted such that any line that uses 
//             : a given id in this column must be *after* the line that 
//             : uses that id in the id column.
//
//   debug     : ignored by the OffsetAnnotator; can contain any information
//             : that is beneficial to a human reading the file

// While the OffsetAnnotationAnnotator is transforming the
// ParsedDocument, it will directly operate on the data structures
// just as if it were the Parser, except that it adds annotations from
// its file as opposed to from the original TokenizedDocument text.

#include "indri/OffsetAnnotationAnnotator.hpp"
#include <fstream>
#include "indri/TagExtent.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/IntervalTree.hpp"
#include <vector>
#include <string.h>
#include <string>
#include <queue>

void indri::parse::OffsetAnnotationAnnotator::open( const std::string& offsetAnnotationsFile ) {
  
  // Only re-load this data if the new file is *different* from
  // the old file.

  if ( _offsetAnnotationsFile.compare( offsetAnnotationsFile ) == 0 )
    return;

  _offsetAnnotationsFile = offsetAnnotationsFile;

  std::cerr << "Loading offset annotations file: " << _offsetAnnotationsFile << std::endl;

  if ( ! _first_open ) {

    _cleanup();
  }
  _first_open = false;

  // Load file, and check consistency.  Ensure that there are no
  // undefined parent ids.

  char buf[65536]; // these may not be big enough someday.
  char field[256];

  std::ifstream in;
  in.open( offsetAnnotationsFile.c_str() );

  int line = 1;

  // Primary parsing loop:

  while ( in.good() || ! in.eof() ) {

    in.getline( buf, sizeof(buf) - 1 );

    if ( buf[0] == '\0' ) break;

    int fieldStart = 0;
    int fieldCount = 0;
    int fieldOffset = 0;
	
    char* docno = NULL;
    char* name = NULL;
    char* s_value = NULL;
    int type = 0;  // TAG = 1, ATTRIBUTE = 2
    UINT64 id = 0;
    UINT64 i_value = 0;
    UINT64 parent = 0;

    int start = 0;
    int length = 0;

    int len = 0;

    for ( char *c = buf + fieldStart; *c != '\0' && fieldCount < 8 && 
	    fieldOffset < sizeof(field); 
	  c++, fieldOffset++ ) {

      if ( *c == '\t' ) {

	field[fieldOffset] = '\0';
	      
	switch ( fieldCount ) {

	case 0: // DOCNO (string)
	  len = strlen( field );
	  docno = new char[len + 1];
	  strncpy( docno, field, len);
	  docno[len] = '\0';
	  _buffers_allocated.push_back( docno );
	  break;

	case 1: // TYPE (flag)
	  if ( *field == 't' || *field == 'T' ) type = 1;
	  else if ( *field == 'a' || *field == 'A' ) type = 2;
	  else {
	    std::cerr << "WARN: Could not understand type specification '" 
		      << field << "' on line " << line 
		      << "; ignoring line." << std::endl;
	    line++;
	    continue;
	  }
	  break;

	case 2: // ID (UINT64)
	  id = parse_UINT64( field, fieldOffset );
	  break;

	case 3: // NAME (string)
	  len = strlen( field );
	  name = new char[len + 1];
	  strncpy( name, field, len );
	  name[len] = '\0';
          // name should be case normalized to lower case.
          for (char *c = name; *c; c++) *c = tolower(*c);
	  _buffers_allocated.push_back( name );
	  break;

	case 4: // START (int)
	  if ( type == 1 ) {
            start = atoi( field );
            if ( start < 0 ) {
              std::cerr << "WARN: tag named '" << name 
                        << "' starting at negative byte offest on line " 
                        << line << "; ignoring line." << std::endl;
              line++;
              continue;
            }
	  }
	  break;

	case 5: // LENGTH (int)
	  if ( type == 1 ) {
            length = atoi( field );
            if ( length <= 0 ) {
              std::cerr << "WARN: tag named '" << name 
                        << "' with zero or negative byte length on line " 
                        << line << "; ignoring line." << std::endl;
              line++;
              continue;
            }
          }
	  break;

	case 6: // VALUE (UINT64 or string)
	  if ( type == 1 ) i_value = parse_UINT64( field, fieldOffset );
	  else { 
	    len = strlen( field );
	    s_value = new char[len + 1];
	    strncpy( s_value, field, len );
	    s_value[len] = '\0';
	    _buffers_allocated.push_back( s_value );
	  }
	  break;

	case 7: // PARENT (UINT64; 0 indicates no parent );
	  parent = parse_UINT64( field, fieldOffset );
	  break;

	}

	fieldCount++;
	fieldStart += ( fieldOffset + 1 );
	fieldOffset = -1;

      } else {

	field[fieldOffset] = *c;
      }
    }

    if ( id == parent ) {

      std::cerr << "WARN: id and parent id are equal on line " << line 
		<< "; ignoring line." << std::endl;
      line++;
      continue;
    }

    // Now process the line we just read in.

    if ( type == 1 ) {
	    
      if ( ! _is_unique_id( id, line ) ) { line++; continue; }

      // Check that parent id is defined

      TagExtent *p_parent = NULL;

      if ( parent != 0 ) {

	p_parent = _getTag( parent );

	if ( p_parent == NULL ) {

	  std::cerr << "WARN: Undefined parent id '" << parent 
		    << "' used on line " << line 
		    << "; ignoring line" << std::endl;

	  line++;
	  continue;

	}
      }

      // Create new TAG

      std::set<indri::parse::TagExtent*>** p = _annotations.find( docno );

      std::set<indri::parse::TagExtent*>* tags = p ? *p : NULL;

      if ( ! tags ) { 

	tags = new std::set<indri::parse::TagExtent*>;
	_annotations.insert( docno, tags );
      }

      TagExtent* te = new TagExtent;
      te->name = name;
      te->number = i_value;
      te->parent = p_parent;

      // Note that this is an abuse of the TagExtent's semantics.  The
      // begin and end fields are intended to be filled with token
      // positions, but here we are inserting byte positions.  In the
      // transform function, all of the annotations for a particular
      // document will be converted to token extents, en masse.  The
      // positions vector from the ParsedDocument is required to make
      // this conversion.
      te->begin = start;
      te->end = start + length;

	  // Conflate tag if necessary
      if ( _p_conflater ) _p_conflater->conflate( te );
	  
      tags->insert( te );
	  _tag_id_map.insert( id, te );

    } else if ( type == 2 ) {

      // Add attribute to existing TAG

      TagExtent* p_te = _getTag( parent );

      if ( p_te == NULL ) {

	std::cerr << "WARN: Attribute for undefined tag id '" << parent
		  << "' appears on line " << line
		  << "; ignoring line" << std::endl;
	line++;
	continue;
      }

      AttributeValuePair avp;
      avp.attribute = name;
      avp.value = s_value;

      p_te->attributes.push_back( avp );
      _attribute_id_map.insert( id, &avp );

    }

    line++;
  }
	
  in.close();
}

indri::api::ParsedDocument* indri::parse::OffsetAnnotationAnnotator::transform( indri::api::ParsedDocument* document ) {

  const char *docno = _getDocno( document ); 
  std::set<indri::parse::TagExtent*>** p;

  // First, check if the annotations for this document have already been
  // converted to use token extents.

  p = _converted_annotations.find( docno );
  std::set<indri::parse::TagExtent*>* converted_tags = p ? *p : NULL;

  if ( ! converted_tags ) {

    // We must do the conversion, then.

    converted_tags = new std::set<indri::parse::TagExtent*>;

    // Check if we have any annotations for this document

    p = _annotations.find( docno );
    std::set<indri::parse::TagExtent*>* raw_tags = p ? *p : NULL;

    if ( raw_tags && ! raw_tags->empty() ) {

      // Do the conversion.
      convert_annotations( raw_tags, converted_tags, document );

    }

    // Store newly converted tags back to the converted annotations table. 
    _converted_annotations.insert( docno, converted_tags );
  }

  // Return right away if there are no annotations for this document.
  if ( converted_tags->empty() ) return document;

  IntervalTree itree;

  // Load existing ParsedDocument tags into the IntervalTree:
	
  for ( indri::utility::greedy_vector<TagExtent>::iterator i = 
	  document->tags.begin(); i != document->tags.end(); i++ ) {
	  
//     std::cerr << "Inserting existing tag: " << (*i).name << " [" 
// 	      << (*i).begin << ", " << (*i).end << "]" << std::endl;
	  
    if ( ! itree.insert( (*i).begin, (*i).end ) ) {

      std::cerr << "There was an error inserting ParsedDocument tag '"
		<< (*i).name << "' [" << (*i).begin << ", "
		<< (*i).end << "] into the IntervalTree for docno '"
		<< docno << "'!  This isn't supposed to happen."
		<< std::endl;
    }
  }

  // Add annotations from the offset annotations file to
  // IntervalTree, in case any of them overlap with the existing
  // annotations.  If they don't, add them to the ParsedDocument
  // rep.

  for ( std::set<TagExtent*>::iterator i = converted_tags->begin(); 
	i != converted_tags->end(); i++ ) {
	  
//     std::cerr << "Inserting new tag: " << (*i)->name << " [" 
// 	      << (*i)->begin << ", " << (*i)->end << "]" << std::endl;
	  
    if ( ! itree.insert( (*i)->begin, (*i)->end ) ) {

#if 0
      std::cerr << "Tag '" << (*i)->name << "' [" << (*i)->begin << ", "
		<< (*i)->end << "] for docno '" << docno 
		<< "' overlaps with an existing annotation; skipping..."
		<< std::endl;
#endif
    } else {

      document->tags.push_back( *(*i) );
    }
  }

  return document;
}

void indri::parse::OffsetAnnotationAnnotator::convert_annotations( std::set<indri::parse::TagExtent*>* raw_tags,
								   std::set<indri::parse::TagExtent*>* converted_tags, 
								   indri::api::ParsedDocument* document ) {

  // At the top of this priority queue will be the tag that closes first.
  std::priority_queue<indri::parse::TagExtent*,std::vector<indri::parse::TagExtent*>,indri::parse::TagExtent::lowest_end_first> active_tags;

  std::set<indri::parse::TagExtent*>::iterator curr_raw_tag = raw_tags->begin();

  long tok_pos = 0;

  for ( indri::utility::greedy_vector<indri::parse::TermExtent>::iterator 
	  token = document->positions.begin(); 
	token != document->positions.end();
	token++, tok_pos++ ) {

    // Check to see if there are any active tags that can be closed.
    // The top element on active_tags will always be the tag that
    // closes first.

    while ( ! active_tags.empty() &&
	    active_tags.top()->end <= (*token).end ) {

      // Tag on top of queue closes before the end of the current
      // token.

      TagExtent *te = active_tags.top();
    
      // If the current active tag ends before the beginning of this
      // token:

      if ( te->end <= (*token).begin ) {

	te->end = tok_pos;

// 	std::cerr << "Closing tag named " << te->name
// 		  << " at tok_pos " << te->end
// 		  << " because tag end at " << te->end
// 		  << " is before token [" << (*token).begin << ", "
// 		  << (*token).end << "]" << std::endl;

      } else { 

	// Current tag ends inside the current token.

	if ( te->end <= (*token).begin + ( (*token).end - (*token).begin )/2 ) {

	  te->end = tok_pos; // Round down to previous token boundary.

	} else {
	  
	  te->end = tok_pos + 1; // Round up to next token boundary.
	  
	}

// 	std::cerr << "Closing tag named " << te->name
// 		  << " at tok_pos " << te->end
// 		  << " because tag end at " << te->end
// 		  << " is inside token [" << (*token).begin << ", "
// 		  << (*token).end << "]" << std::endl;

      }


//       std::cerr << "Closed Tag named " << te->name 
// 	        << " at tok_pos " << te->end << std::endl;

      converted_tags->insert( te );
      active_tags.pop();
    }

    // Now check to see if there are any tags that can be activated at
    // this token position.  Tags in raw_tags are sorted in first
    // and longest order.  To be able to activate a tag here, that tag
    // must begin before the current token ends.  Said in another way,
    // the tag must begin before the token begins or inside the token
    // to be activated here.

    while ( curr_raw_tag != raw_tags->end() && (*curr_raw_tag)->begin < (*token).end ) {

      // Tags that begin AND end before the current token are zero
      // token-length tags and must be skipped.
      if ( (*curr_raw_tag)->end <= (*token).begin ) {

// 	std::cerr << "Tag named " << (*curr_raw_tag)->name 
// 		  << " beginning at byte offset " << (*curr_raw_tag)->begin
// 		  << " and ending at byte offset " << (*curr_raw_tag)->end
// 		  << " encloses no tokens and will be ignored."
// 		  << std::endl;

	curr_raw_tag++;
	continue;
      }

      // To activate a tag, create a copy of it and insert that copy
      // into active_tags

      TagExtent* te = new TagExtent();
      te->name = (*curr_raw_tag)->name;
      te->number = (*curr_raw_tag)->number;
      te->parent = (*curr_raw_tag)->parent;
      te->attributes = (*curr_raw_tag)->attributes;

      // When the tag begins in the middle of the token, we need to
      // decide whether to round up (activate the tag at this token
      // position) or round down (activate the tag at tok_pos + 1).

      if ( (*curr_raw_tag)->begin <= (*token).begin + ( (*token).end - (*token).begin )/2 ) {

	// Tag either begins before the token, or is closer to begin
	// than to the end of the token, so we are rounding up.  Begin
	// value will be be set to the current token position.
	te->begin = tok_pos;

      } else {

	// Tag begins closer to where the token ends, so we'll round down.
	te->begin = tok_pos + 1;

      }

      // End value must be filled in when the tag is closed; for now,
      // we just store the byte offset of the end of the tag.
      te->end = (*curr_raw_tag)->end;
     
      active_tags.push( te );

//       std::cerr << "Activated Tag named " << (*curr_raw_tag)->name << " with byte range [" 
// 		<< (*curr_raw_tag)->begin << ", " << (*curr_raw_tag)->end 
// 		<< "] activated at tok_pos " << te->begin << std::endl;

      // Move onto the next tag
      curr_raw_tag++;

    }
  }

  // Close any tags that remain on the active_tags list.

  while ( ! active_tags.empty() ) {

    TagExtent *te = active_tags.top();
    te->end = tok_pos;

//     std::cerr << "Closed Tag named " << te->name 
// 	      << " at tok_pos " << te->end << std::endl;

    converted_tags->insert( te );
    active_tags.pop();
    
  }
}
