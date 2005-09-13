/*==========================================================================
 * Copyright (c) 2000-2004 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software (and below), and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

#ifndef _INDRITEXTHANDLER_HPP
#define _INDRITEXTHANDLER_HPP

#include "Parser.hpp"
#include "indri/ParsedDocument.hpp"
#include "indri/IndexEnvironment.hpp"

namespace lemur 
{
  namespace parse 
  {
    
    /**
       This class creates a ParsedDocument object that is compatible with an
       IndriIndex and pushes is into the index.
    */

#define DOCIDKEY "docno"

    class IndriTextHandler : public lemur::api::TextHandler {

    public:
      /// Create an IndriIndex (repository) with the given name
      /// Pass in the parser being used at the beginning of the chain
      IndriTextHandler(const string &name, int memory, 
                       const lemur::api::Parser* p);
      ~IndriTextHandler();

      /// Signal the start of a document
      char * handleDoc(char * docno);
      /// Handle end of document
      void handleEndDoc();
      /// Handles a word to be indexed
      char * handleWord(char * word, const char* original, PropertyList* list);
      char * handleBeginTag(char* tag, const char* orig, PropertyList* props);
      char * handleEndTag(char* tag, const char* orig, PropertyList* props);

    protected:
      /// The indexer.
      indri::api::IndexEnvironment env;
      /// Document properties object to be reused over repeated calls.
      indri::api::ParsedDocument document;
      /// Handle the document id
      indri::parse::MetadataPair docid;
      /// the current document id
      char* curdocno;
      /// the beginning byte offset into file of this documents
      int docbegin;
      /// We need a handle to the parser to get file byte locations
      const lemur::api::Parser* parser;
      /// Handle tag extents
    };
  }
}

#endif

