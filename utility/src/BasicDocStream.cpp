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



void BasicTokenDoc::startTermIteration() 
{
  curWord = buf1;
  // peek one term
  *docStr >> curWord;
}

TokenTerm * BasicTokenDoc::nextTerm()
{
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


void BasicTokenDoc::readID()
{
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


BasicDocStream::BasicDocStream (const char * inputFile)
{
  strcpy(file, inputFile);
  ifs = new ifstream(inputFile, ios::in);
  if (ifs->fail() ) {
    throw Exception("BasicDocStream", "can't open BasicDocStream source file");
  }
}


bool BasicDocStream::hasMore()
{
  streampos pos = ifs->tellg();
  bool moreData= (*ifs >> buf);
  ifs->seekg(pos);
  return moreData; 
}


void BasicDocStream::startDocIteration()
{
  ifs->close();
  ifs->open(file);
  ifs->seekg(0);
  ifs->clear(); 
}


Document *BasicDocStream::nextDoc()
{
  static BasicTokenDoc doc(ifs);
  doc.readID();
  return &doc;
}

