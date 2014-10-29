/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#ifndef _WRITERTEXTHANDLER_HPP
#define _WRITERTEXTHANDLER_HPP
#include "TextHandler.hpp"

#include <fstream>
#include "common_headers.hpp"
namespace lemur 
{
  namespace parse 
  {
    
    ///
    ///  Outputs text in a format that can be used by 
    ///  RetEval (for queries) or BuildBasicIndex (for documents).
    ///  This class is a destination TextHandler.
    ///
    ///


    class WriterTextHandler : public lemur::api::TextHandler {

    public:

      /// Create and set the file to write to.
      WriterTextHandler(const string &filename);
      ~WriterTextHandler();

      /// Begin a doc.
      char * handleDoc(char * docno);
      /// Add a word to the doc.
      char * handleWord(char * word);


    private:
      /// end a document
      void endDoc();

      /// First doc?
      bool first;

      /// Output file.
      ofstream * outfile;
    };
  }
}

#endif
