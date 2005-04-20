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
// WordDocumentExtractor
//
// 14 June 2004 -- tds
//
// Code is based in part on the AutoWord class
// by Poonam Bajaj.
//
#ifndef INDRI_WORDDOCUMENTEXTRACTOR_HPP
#define INDRI_WORDDOCUMENTEXTRACTOR_HPP

#include "lemur-compat.hpp"
#include "indri/Buffer.hpp"
#include "indri/UnparsedDocument.hpp"
#include "Exception.hpp"
#include <string>
#include "indri/DocumentIterator.hpp"

#ifdef WIN32
namespace indri
{
  namespace parse
  {
    
    class WordDocumentExtractor : public DocumentIterator {
      void* _internal;

      indri::utility::Buffer _documentTextBuffer;
      UnparsedDocument _unparsedDocument;
      std::string _documentPath;
      bool _documentWaiting;

      void initialize();
      void uninitialize();
  
    public:
      WordDocumentExtractor();
      ~WordDocumentExtractor();

      void open( const std::string& filename );
      UnparsedDocument* nextDocument( );
      void quit();
      void close();
    };
  }
}

#endif
#endif // INDRI_WORDDOCUMENTEXTRACTOR_HPP
