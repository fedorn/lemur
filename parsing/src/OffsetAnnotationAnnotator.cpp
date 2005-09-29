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

void indri::parse::OffsetAnnotationAnnotator::open( const std::string& offsetAnnotationsFile ) {
  
  // Only re-load this data if the new file is *different* from
  // the old file.

  if ( _offsetAnnotationsFile.compare( offsetAnnotationsFile ) == 0 )
    return;

  _offsetAnnotationsFile = offsetAnnotationsFile;

  if ( ! _first_open ) {

    _cleanup();
  }
  _first_open = false;

  // Load file, and check consistency.  Ensure that there are no
  // undefined parent ids.

  char buf[65536];
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
	  // Prepare this document's IntervalTree
	  _itree.insert( docno, new IntervalTree );
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
	  if ( type == 1 ) start = atoi( field );
	  break;

	case 5: // LENGTH (int)
	  if ( type == 1 ) length = atoi( field );
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

      indri::utility::greedy_vector<indri::parse::TagExtent*>** p =
	_annotations.find( docno );

      indri::utility::greedy_vector<indri::parse::TagExtent*>* tags =
	p ? *p : NULL;

      if ( ! tags ) { 

	tags = new indri::utility::greedy_vector<indri::parse::TagExtent*>;
	_annotations.insert( docno, tags );
      }

      TagExtent* te = new TagExtent;
      te->name = name;
      te->begin = start;
      te->end = start + length;
      te->number = i_value;
      te->parent = p_parent;

      // Overlapping tag check:

      IntervalTree** q = _itree.find( docno );
      IntervalTree* itree = q ? *q : NULL;

      if ( itree ) {

	if ( itree->insert( te->begin, te->end ) ) {

	  // Conflate tag if necessary
	  if ( _p_conflater )
	    _p_conflater->conflate( te );
	  
	  tags->push_back( te );
	  _tag_id_map.insert( id, te );

	} else {

	  std::cerr << "Overlapping tag ( id=" << id 
		    << ", name='" << te->name 
		    << "' ) detected; skipping..." << std::endl;
	  
	  itree->walk_tree( std::cerr );

	  //exit(-1); // DEBUG
	}
      } else {

	std::cerr << "IntervalTree is null!" << std::endl;
      }
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

  // Check that we have any annotations for this document (lookup by docno)

  const char *docno = _getDocno( document ); 

  indri::utility::greedy_vector<indri::parse::TagExtent*>** p =
    _annotations.find( docno );

  indri::utility::greedy_vector<indri::parse::TagExtent*>* tags =
    p ? *p : NULL;

  if ( ! tags || tags->size() == 0 ) return document;

  IntervalTree itree;

  // Load existing ParsedDocument tags into the IntervalTree:
	
  for ( indri::utility::greedy_vector<TagExtent>::iterator i = 
	  document->tags.begin(); i != document->tags.end(); i++ ) {
	  
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

  for ( indri::utility::greedy_vector<TagExtent*>::iterator i = 
	  tags->begin(); i != tags->end(); i++ ) {
	  
    if ( ! itree.insert( (*i)->begin, (*i)->end ) ) {

      std::cerr << "Tag '" << (*i)->name << "' [" << (*i)->begin << ", "
		<< (*i)->end << "] for docno '" << docno 
		<< "' overlaps with an existing annotation; skipping..."
		<< std::endl;

    } else {

      // std::cout << "Inserted tag" << std::endl; 

      document->tags.push_back( *(*i) );
    }
  }

  return document;
}
