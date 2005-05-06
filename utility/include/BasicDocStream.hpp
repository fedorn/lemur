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


#ifndef _BASICFILESTREAM_HPP
#define _BASICFILESTREAM_HPP
#include "common_headers.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "DocStream.hpp"
#include "Exception.hpp"


namespace lemur 
{
  namespace parse 
  {
    
    /// A basic DocStream implementation

    /*!

    BasicDocStream is an implementation of DocStream that recognizes
    the following format:

    <PRE>
    &lt;DOC unique_document_identifier&gt;
    this
    is
    an
    example
    &lt;/DOC&gt;
    </PRE>


    The following is a typical example of using BasicDocStream(or DocSTream)
    :

    <PRE>

    DocStream *docStr = new BasicDocStream("source");
  
    docStr->startDocIteration();

    while (docStr->hasMore()) {

    Document *doc = docStr->nextDoc();
    cout << "doc id: "<< doc->getID() << endl;
    doc->startTermIteration();
    while (doc->hasMore()) {
    TokenTerm *term = thisDoc->nextTerm();
    cout << "term: "<< term->spelling() << endl;
    }
    }
    </PRE>
    */


#define MAXLINE 65536


    /// doc representation for BasicDocStream

    class BasicTokenDoc : public lemur::api::Document {
    public:
      BasicTokenDoc() {
      }
      BasicTokenDoc(ifstream *stream): docStr(stream) {
      }
      void startTermIteration() const;  
  
      const char *getID() const { return id;}

      bool hasMore() const{ return (strcmp(curWord, "</DOC>") != 0);}
    
      const lemur::api::Term * nextTerm() const;

      void skipToEnd() const;
      friend class BasicDocStream;
    private:
      void readID(); 
      mutable char *curWord;
      mutable char buf1[20000];
      mutable char buf2[20000];
      char id[2000];
      ifstream *docStr;
      streampos startPos; // starting position of the terms in the file
      //replace  static BasicTokenTerm t; with attribute
      mutable lemur::api::Term t;
    };


    /// A DocStream handler of a stream with the basic lemur format
    class BasicDocStream : public lemur::api::DocStream
    {
    public:
      BasicDocStream() {}
      BasicDocStream (const string &inputFile);

      virtual ~BasicDocStream() {  delete ifs;}

    public:
        
      bool hasMore(); 

      void startDocIteration();

      lemur::api::Document *nextDoc();

    private:
      char file[1024];
      ifstream *ifs;
      char buf[2000];
      bool nextTokenRead;
      // replace static BasicTokenDoc doc;  with attribute
      BasicTokenDoc doc;
    };
  }
}
#endif
