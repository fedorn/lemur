/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _BASICFILESTREAM_HPP
#define _BASICFILESTREAM_HPP

/// A basic DocStream implementation

/*!

  BasicDocStream is an implementation of DocStream that recognizes
  the following format:

  <PRE>
   <DOC unique_document_identifier>
   this
   is
   an
   example
   </DOC>
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

#include "common_headers.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "DocStream.hpp"
#include "Exception.hpp"


#define MAXLINE 65536



class BasicTokenTerm : public TokenTerm {
 public:
  BasicTokenTerm() {}
  virtual ~BasicTokenTerm() {}
  virtual const char *spelling() { return str;}
  friend class BasicTokenDoc;
 private:
  char *str;
};


class BasicTokenDoc : public Document {
 public:
  BasicTokenDoc(ifstream *stream): docStr(stream) {
  }
   void startTermIteration() {
    curWord = buf1;
    // peek one term
    *docStr >> curWord;

  }
  
  char *getID() const { return (char *)id;}

  bool hasMore() { return (strcmp(curWord, "</DOC>") != 0);}
    
  TokenTerm * nextTerm() {
    static BasicTokenTerm t;
    t.str = curWord;
    if (curWord == buf1) {
      curWord = buf2;
    } else {
      curWord = buf1;
    }
    *docStr >> curWord;
    return &t;
  }

  friend class BasicDocStream;
 private:
  void readID() {
    // get id
    *docStr >> buf1;
    if (strcmp(buf1, "<DOC")) {
      cerr << " actual token seen: "<< buf1 << endl;
      throw Exception("BasicTokenDoc", "begin doc marker expected");
    }
    *docStr >> id;
    int len= strlen(id);
    if (id[len-1]!='>') {
      throw Exception("BasicTokenDoc","ill-formatted doc id, > expected");
    }
    id[len-1]='\0';
  }

  char *curWord;
  char buf1[20000];
  char buf2[20000];
  char id[2000];
  ifstream *docStr;
};


class BasicDocStream : public DocStream
{
public:
  BasicDocStream() {}
  BasicDocStream (const char * inputFile) {
    strcpy(file, inputFile);
    ifs = new ifstream(inputFile, ios::in);
    if (ifs->fail() ) {
      throw Exception("BasicDocStream", "can't open BasicDocStream source file");
    }
  }
  virtual ~BasicDocStream() {  delete ifs;}

public:
	
  bool hasMore() {
    streampos pos = ifs->tellg();
    bool moreData= (*ifs >> buf);
    ifs->seekg(pos);
    return moreData; }
 

  void startDocIteration() {
    ifs->close();
    ifs->open(file);
     ifs->seekg(0);
     ifs->clear(); 
  }

  Document *nextDoc() {
    static BasicTokenDoc doc(ifs);
    doc.readID();
    return &doc;
  }

 private:
  char file[1024];
  ifstream *ifs;
  char buf[2000];
};




#endif




