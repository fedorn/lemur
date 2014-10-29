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


#ifndef _DOCSTREAM_HPP
#define _DOCSTREAM_HPP

#include "Document.hpp"
#include "common_headers.hpp"

namespace lemur 
{
  namespace api 
  {
    
    /// \brief Abstract interface for a collection of documents

    /*!
      DocStream is an abstract interface for a collection of documents.  A given
      realization can have special tokenization, document header
      formats, etc, and will return a special Document instance to
      indicate this. 

      The following is an example of supporting an index with position information:

      An example of supporting index with position information
      <PRE>
   
      // a DocStream that handles position
      class PosDocStream : public DocStream {
      ... 
      Document *nextDoc() {
      return (new PosDocument(...)); // returns a special Document
      } 
      ...
      };

      // a Document that has position information 
      class PosDocument : public Document {
      ... 
      TokenTerm  *nextTerm() {
      return (new PosTerm(...)); // returns a special Term
      }
      };

      // a Term that has position 
      class PosTerm: public TokenTerm {
      int getPosition() { 
      ...
      }
      };
 
      // Indexer that records term positions
      class PosIndex : public Index {
      ...
      PosDocStream *db;

      ... // when indexing
 
      db->startDocIteration();
      Document *doc;
      while (db->hasMore()) {
      Document *doc = db->nextDoc(); // we'll actually get a PosDocument
      doc->startTermIteration();
      PosTerm *term;
      while (doc->hasMore()) {
      term = (PosTerm *)nextTerm(term); 
      // note that down-casting!
      term->getPosition();
      term->spelling();
      ...
             
      }
      }
      ... 
      }
      </PRE>


    */

    class DocStream {
    public:

      virtual ~DocStream(){};
 
      /// start document iteration
      virtual void startDocIteration()=0;

      virtual bool hasMore()=0;

      /// return a pointer to next document (static memory, do not delete returned instance).  hasMore() should be called before calling nextDoc()
      virtual Document * nextDoc()=0;
    };
  }
}
#endif
