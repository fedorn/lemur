 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 * The Lemur toolkit for language modeling and information retrieval.
 * Copyright (c) 2001 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for research or educational purposes,
 * provided that this copyright notice is maintained and note is made
 * of any changes to the source code.
 * 
 * This is a research system.  The code is distributed on an "as is" basis,
 * without any warranty, express or implied. 
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _DOCSTREAM_HPP
#define _DOCSTREAM_HPP

#include "Document.hpp"

/// Abstract interface for a collection of documents

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

#include <stdlib.h>
#include <iostream.h>


class DocStream {
public:

  virtual ~DocStream(){};

  /*!
  Typical usage: @see Document
    
    DocStream &myStream;\n
    ...\n
    myStream.startDocIteration();\n
    Document *doc;\n
    while (myStream.nextDoc(doc)) {\n
      Term *term;\n
      doc->startTermIteration();\n
      while (doc->nextTerm(term)) {\n
      ... process "term" ...\n
      delete term;
      }\n
      delete doc;\n
    }\n
  */

 
  //@{ @name Document Iteration

  /// start document iteration
  virtual void startDocIteration()=0;

  virtual bool hasMore()=0;

  /// next document, caller responsible for deleting doc and should call hasMore() before calling nextDoc()
  virtual Document * nextDoc()=0;


  //@}


};





#endif


  


