/*==========================================================================
 * Copyright (c) 2009 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


//
// WARCDocumentIterator
//
// 03 Mar 2009 -- dmf
//

#ifndef INDRI_WARCDOCUMENTITERATOR_HPP
#define INDRI_WARCDOCUMENTITERATOR_HPP

#include <string>
#include <fstream>
#include "indri/DocumentIterator.hpp"
#include "indri/Buffer.hpp"
#include "indri/UnparsedDocument.hpp"

namespace indri
{
  namespace parse
  {
    
    class WARCDocumentIterator : public DocumentIterator {
    private:
      UnparsedDocument _document;
      FILE* _in;
      indri::utility::Buffer _buffer;
      indri::utility::Buffer _metaBuffer;
      std::string _fileName;
      const char * _recordID;
      const char * _contentLength;
      const char *_targetURI;
      const char *_trecID;
      int _recordIDLength;
      int _contentLengthLength;
      int _targetURILength;
      int _trecIDLength;
      std::string _warcUUID;
      const char * _warcMeta;
      const char * _dochdr;
      const char * _docnoString;
      char _docno[512];

      bool _readLine( char*& beginLine, size_t& lineLength );

    public:
      WARCDocumentIterator();
      ~WARCDocumentIterator();
      void open( const std::string& filename );
      void close();
      UnparsedDocument* nextDocument();
    };
  }
}

#endif // INDRI_WARCDOCUMENTITERATOR_HPP
