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


#include <cstring>
#include <cctype>
#include <cassert>
#include "BasicDocStream.hpp"



void BasicTokenDoc::startTermIteration() const
{
  // ensure the start position of the terms
  docStr->seekg(startPos);
  curWord = buf1;
  // peek one term
  *docStr >> curWord;
}

void BasicTokenDoc::skipToEnd() const
{
  startTermIteration();
  while (hasMore()) {
    nextTerm();
  }
}

const TokenTerm * BasicTokenDoc::nextTerm() const
{
  //  static BasicTokenTerm t;
  t.str = curWord;
  if (curWord == buf1) {
    curWord = buf2;
  } else {
    curWord = buf1;
  }
  *docStr >> curWord;
  return &t;
}


void BasicTokenDoc::readID()
{
  // get id
  *docStr >> id;
  int len= strlen(id);
  if (id[len-1]!='>') {
    throw Exception("BasicTokenDoc","ill-formatted doc id, > expected");
  }
  id[len-1]='\0';
  startPos = docStr->tellg(); // record the start position of terms
}


BasicDocStream::BasicDocStream (const string &inputFile)
{
  strcpy(file, inputFile.c_str());
  ifs = new ifstream(file, ios::in);
  if (ifs->fail() ) {
    throw Exception("BasicDocStream", "can't open BasicDocStream source file");
  }
}


bool BasicDocStream::hasMore()
{
  bool moreDoc;
  if (!nextTokenRead) {
    moreDoc = *ifs >> buf;
    nextTokenRead = true;
    if (moreDoc && strcmp(buf, "<DOC")) {
      cerr << " actual token seen: "<< buf << endl;
      throw Exception("BasicDocStream", "begin doc marker expected");
    }
  }

  return moreDoc; 
}


void BasicDocStream::startDocIteration()
{
  ifs->close();
  ifs->open(file);
  ifs->seekg(0);
  ifs->clear(); 
  nextTokenRead =false;
}

Document *BasicDocStream::nextDoc()
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

