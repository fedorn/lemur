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

#ifndef INDRI_OFFSETANNOTATIONANNOTATOR_HPP
#define INDRI_OFFSETANNOTATIONANNOTATOR_HPP

#include "indri/Buffer.hpp"
#include "indri/Transformation.hpp"
#include <iostream>
#include "indri/TagExtent.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/IntervalTree.hpp"
#include <vector>
#include <string.h>
#include <string>
#include "indri/HashTable.hpp"
#include "indri/greedy_vector"
#include "indri/Conflater.hpp"

namespace indri {
  namespace parse {

    class OffsetAnnotationAnnotator : public Transformation {

    private:
      std::string _offsetAnnotationsFile;

      indri::utility::HashTable<const char *,indri::utility::greedy_vector<TagExtent*>*> _annotations;
      indri::utility::HashTable<UINT64,TagExtent*> _tag_id_map;
      indri::utility::HashTable<UINT64,AttributeValuePair*> _attribute_id_map;
      std::vector<char *> _buffers_allocated;
      bool _first_open;

      // IntervalTrees for checking tag overlapping.  These are
      // cleared every time the open() method is called.  Tags read
      // from the offset annotations file are checked in the open()
      // method, and tags present in the ParsedDocument fed to the
      // transform() method are checked in that method.

      indri::utility::HashTable<const char*,IntervalTree*> _itree;

      ObjectHandler<indri::api::ParsedDocument>* _handler;
      Conflater* _p_conflater;
      
      const char *_getDocno( indri::api::ParsedDocument* document ) {

        // find DOCNO attribute in document

        for ( size_t i=0; i<document->metadata.size(); i++ ) {
          const char* attributeName = document->metadata[i].key;
          const char* attributeValue = (const char*) document->metadata[i].value;

          if ( ! strcmp( attributeName, "docno" ) ) return attributeValue;
	}
 
        return NULL;
      }


      TagExtent *_getTag( UINT64 id ) {

	// return TagExtent corresponding to given id, or NULL if invalid id.

	if ( id == 0 ) return NULL;

	TagExtent** p = _tag_id_map.find( id );

	if ( ! p ) return NULL;
	else return *p;
      }
     
      AttributeValuePair *_getAttribute( UINT64 id ) {

	// return Attribute corresponding to given id, or NULL if invalid id.

	if ( id == 0 ) return NULL;

	AttributeValuePair** p = _attribute_id_map.find( id );

	if ( ! p ) return NULL;
	else return *p;
      }

      bool _is_unique_id( UINT64 id, int line ) {

	// Make sure ID has not already been used.

	if ( id == 0 ) {

	  std::cerr << "WARN: Invalid id '0' used on line " << line 
		    << "; ignoring line." << std::endl;
	  return false;
	}

	if ( _getTag( id ) || _getAttribute( id ) ) {

	  std::cerr << "WARN: Id '" << id << "' redefined on line " << line 
		    << "; ignoring line." << std::endl;
	  return false;
	}

	return true;
      }

      UINT64 parse_UINT64( const char *str, int n ) {

	UINT64 result = 0;
	int i = 0;
	for ( const char* c = str; i < n & c != '\0'; c++, i++ )
	  result = result * 10 + ( *c - '0' );

	return result;
      }

      void _cleanup() {
	
	// Cleanup _annotations, _tag_id_map, _attribute_id_map and
	// _itree in preparation for object destruction, or for an
	// open call on a new offset annotations file.

	for ( indri::utility::HashTable<const char *,indri::utility::greedy_vector<TagExtent*>*>::iterator i = _annotations.begin(); i != _annotations.end(); i++ ) {

	  indri::utility::greedy_vector<TagExtent*>* p_vec = *(*i).second;

	  for ( indri::utility::greedy_vector<TagExtent*>::iterator j = 
		  p_vec->begin(); j != p_vec->end(); j++ ) {

	    delete (*j); // TagExtent
	  }
	}

	_annotations.clear();

	// Note: every TagExtent pointed to by an element of the
	// _tag_id_map, and every AttributeValuePair pointed to by an
	// element of the _attribute_id_map will have already been
	// deleted above.

	_tag_id_map.clear();
	_attribute_id_map.clear();

	// Clean up IntervalTrees 

	for ( indri::utility::HashTable<const char*,IntervalTree*>::iterator i =
		_itree.begin(); i != _itree.end(); i++ ) {

	  delete (*i).second; // IntervalTree
	}

      }

    public:
      OffsetAnnotationAnnotator( Conflater* p_conflater ) { 

        _handler = NULL;
	_p_conflater = p_conflater;
	_first_open = true;
      }

      OffsetAnnotationAnnotator() {

	_handler = NULL;
	_p_conflater = NULL;
      }

      ~OffsetAnnotationAnnotator() {

	_cleanup();

	for ( std::vector<char *>::iterator i = _buffers_allocated.begin();
	      i != _buffers_allocated.end(); i++ )
	  delete[] (*i);

	delete _p_conflater;
      }

      void setHandler( ObjectHandler<indri::api::ParsedDocument>& handler ) {

        _handler = &handler;
      }

      void handle( indri::api::ParsedDocument* document ) {

        _handler->handle( transform( document ) );
      }

      // Defined in OffsetAnnotationAnnotator.cpp
      void open( const std::string& offsetAnnotationsFile );
      indri::api::ParsedDocument* transform( indri::api::ParsedDocument* document );

    };

  }
}

#endif // INDRI_OFFSETANNOTATIONANNOTATOR_HPP

