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
// TextDocumentExtractor
//
// 16 August 2004 -- tds
//

#ifndef INDRI_TEXTDOCUMENTEXTRACTOR_HPP
#define INDRI_TEXTDOCUMENTEXTRACTOR_HPP

#include "indri/DocumentIterator.hpp"
#include "indri/UnparsedDocument.hpp"
#include "indri/Buffer.hpp"
#include <fstream>

class TextDocumentExtractor : public DocumentIterator {
private:
  std::string _filename;
  UnparsedDocument _document;
  Buffer _buffer;
  std::ifstream _in;

public:
  void open( const std::string& filename );
  UnparsedDocument* nextDocument();
  void close();
};

#endif // INDRI_TEXTDOCUMENTEXTRACTOR_HPP
