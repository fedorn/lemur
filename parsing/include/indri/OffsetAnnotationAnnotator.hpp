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

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <utility>

#include "indri/Buffer.hpp"
#include "indri/Transformation.hpp"
#include "indri/TagExtent.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/HashTable.hpp"
#include "indri/greedy_vector"
#include "indri/Conflater.hpp"

namespace indri {
  namespace parse {

    class OffsetAnnotationAnnotator : public Transformation {

    private:
      std::string _offsetAnnotationsFile;

      // Before the actual ParsedDocument is read in, we can not
      // convert byte extents from the .oa file to token extents.  The
      // TagExtents in this table have their begin and end values
      // expressed as byte extents, not token extents.
      indri::utility::HashTable<const char *,std::set<TagExtent*>*> _annotations;

      // After a document's set of annotations has been converted
      // to token extents, we store the result in this table in case
      // someone asks for that same document's annotations again.
      indri::utility::HashTable<const char *,std::set<TagExtent*>*> _converted_annotations;

      indri::utility::HashTable<UINT64,TagExtent*> _tag_id_map;
      indri::utility::HashTable<UINT64,AttributeValuePair*> _attribute_id_map;
      std::vector<char *> _buffers_allocated;
      bool _first_open;

      ObjectHandler<indri::api::ParsedDocument>* _handler;
      Conflater* _p_conflater;
      
      const char *_getDocno( indri::api::ParsedDocument* document ) {

        // find DOCNO attribute in document
	const char *retVal = NULL;
        for ( size_t i=0; i<document->metadata.size(); i++ ) {
          const char* attributeName = document->metadata[i].key;
          const char* attributeValue = (const char*) document->metadata[i].value;

          if ( ! strcmp( attributeName, "docno" ) ) retVal = attributeValue;
        }
 
        return retVal;
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
        for ( const char* c = str; i < n && c != '\0'; c++, i++ )
          result = result * 10 + ( *c - '0' );

        return result;
      }

      void _cleanup() {
        // clear any allocated buffers
        for ( std::vector<char *>::iterator i = _buffers_allocated.begin();
              i != _buffers_allocated.end(); i++ )
          delete[] (*i);

        _buffers_allocated.clear();
        
        // Cleanup _annotations, _converted_annotations, _tag_id_map,
        // and _attribute_id_map in preparation for object
        // destruction, or for an open call on a new offset
        // annotations file.

        for ( indri::utility::HashTable<const char *,std::set<TagExtent*>*>::iterator i = _annotations.begin(); i != _annotations.end(); i++ ) {

          std::set<TagExtent*>* p_set = *(*i).second;

          for ( std::set<TagExtent*>::iterator j = p_set->begin(); 
                j != p_set->end(); j++ ) {

            delete (*j); // TagExtent
          }
          delete(p_set);
        }

        _annotations.clear();

        for ( indri::utility::HashTable<const char *,std::set<TagExtent*>*>::iterator i = _converted_annotations.begin(); i != _converted_annotations.end(); i++ ) {

          std::set<TagExtent*>* p_set = *(*i).second;

          for ( std::set<TagExtent*>::iterator j = p_set->begin(); 
                j != p_set->end(); j++ ) {

            delete (*j); // TagExtent
          }
          delete (*i).first;
          delete(p_set);
        }

        _converted_annotations.clear();

        // Note: every TagExtent pointed to by an element of the
        // _tag_id_map, and every AttributeValuePair pointed to by an
        // element of the _attribute_id_map will have already been
        // deleted above.

        _tag_id_map.clear();
 //     for ( indri::utility::HashTable<UINT64,AttributeValuePair*>::iterator i = _attribute_id_map.begin(); i != _attribute_id_map.end(); i++ ) {
//        delete (*i).second;
//      }
        _attribute_id_map.clear();

      }

      void convert_annotations( std::set<indri::parse::TagExtent*>* raw_tags,
                                std::set<indri::parse::TagExtent*>* converted_tags, 
                                indri::api::ParsedDocument* document );

    public:
      OffsetAnnotationAnnotator( Conflater* p_conflater ) { 

        _handler = NULL;
        _p_conflater = p_conflater;
        _first_open = true;
      }

      OffsetAnnotationAnnotator() {

        _handler = NULL;
        _p_conflater = NULL;
        _first_open = true;
      }

      ~OffsetAnnotationAnnotator() {
        _cleanup();
      }

      void setTags (const char *docno, const std::vector<indri::parse::TagExtent *> &tagset);
      
      void setConflater(Conflater* p_conflater) 
      {
        _p_conflater = p_conflater;
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

