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


#include <cstring>
#include <cctype>
#include <cassert>
#include "BasicDocStream.hpp"



void lemur::parse::BasicTokenDoc::startTermIteration() const
{
  // ensure the start position of the terms
  docStr->seekg(startPos);
  curWord = buf1;
  // peek one term
  *docStr >> curWord;
}

void lemur::parse::BasicTokenDoc::skipToEnd() const
{
  startTermIteration();
  while (hasMore()) {
    nextTerm();
  }
}

const lemur::api::Term * lemur::parse::BasicTokenDoc::nextTerm() const
{
  //  static BasicTokenTerm t;
  t.spelling(curWord);
  if (curWord == buf1) {
    curWord = buf2;
  } else {
    curWord = buf1;
  }
  *docStr >> curWord;
  return &t;
}


void lemur::parse::BasicTokenDoc::readID()
{
  // get id
  *docStr >> id;
  int len= strlen(id);
  if (id[len-1]!='>') {
    throw lemur::api::Exception("BasicTokenDoc","ill-formatted doc id, > expected");
  }
  id[len-1]='\0';
  startPos = docStr->tellg(); // record the start position of terms
}


lemur::parse::BasicDocStream::BasicDocStream (const string &inputFile)
{
  strcpy(file, inputFile.c_str());
  ifs = new ifstream(file, ios::in);
  if (ifs->fail() ) {
    throw lemur::api::Exception("BasicDocStream", "can't open BasicDocStream source file");
  }
}


bool lemur::parse::BasicDocStream::hasMore()
{
  bool moreDoc = false;
  if (!nextTokenRead) {
    moreDoc = *ifs >> buf;
    nextTokenRead = true;
    if (moreDoc && strcmp(buf, "<DOC")) {
      cerr << " actual token seen: "<< buf << endl;
      throw lemur::api::Exception("BasicDocStream", "begin doc marker expected");
    }
  }

  return moreDoc; 
}


void lemur::parse::BasicDocStream::startDocIteration()
{
  ifs->close();
  ifs->open(file);
  ifs->seekg(0);
  ifs->clear(); 
  nextTokenRead =false;
}

lemur::api::Document *lemur::parse::BasicDocStream::nextDoc()
{
  // fails to initialize properly, preventing reuse of a 
  // BasicDocStream (or opening more than one).
  // static BasicTokenDoc doc(ifs);
  // make it an instance attribute
  //  static BasicTokenDoc doc;
  doc.docStr = ifs;
  doc.readID();
  nextTokenRead = false;
  return &doc;
}

