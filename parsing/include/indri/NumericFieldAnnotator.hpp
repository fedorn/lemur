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
// NumericFieldAnnotator
//
// 25 May 2004 -- tds
//

#ifndef INDRI_NUMERICFIELDANNOTATOR_HPP
#define INDRI_NUMERICFIELDANNOTATOR_HPP
namespace indri
{
  namespace parse
  {

    class NumericFieldAnnotator : public Transformation {
    private:
      ObjectHandler<indri::api::ParsedDocument>* _handler;
      std::string& _field;

    public:
      NumericFieldAnnotator( std::string& field ) :
        _handler(0),
        _field(field)
      {
      }

      indri::api::ParsedDocument* transform( indri::api::ParsedDocument* document ) {
        for( size_t i=0; i<document->tags.size(); i++ ) {
          TagExtent& extent = document->tags[i];

          if( _field == extent.name ) {
            char* numberText = document->terms[ extent.begin ]; 
            INT64 value = string_to_i64( numberText );
            extent.number = value;
          }
        }

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

#endif // INDRI_NUMERICFIELDANNOTATOR_HPP


