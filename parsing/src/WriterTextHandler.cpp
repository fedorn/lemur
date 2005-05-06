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

#include "WriterTextHandler.hpp"
#include <cctype>


lemur::parse::WriterTextHandler::WriterTextHandler(const string &filename) {
  // create the output file
  outfile = new ofstream(filename.c_str());
  // set state in first doc
  first = true;
}

lemur::parse::WriterTextHandler::~WriterTextHandler() {
  endDoc();
  // close the output file
  outfile->close();
  delete outfile;
}


char * 
lemur::parse::WriterTextHandler::handleDoc(char * docno) {
  // output the end doc tag if not the first doc
  if (first) {
    first = false;
  } else {
    endDoc();
  }
  // begin the doc
  *outfile << "<DOC " << docno << ">" << endl;
  return docno;
}

char * 
lemur::parse::WriterTextHandler::handleWord(char * word) {
  // output the word
  if (word != NULL) {
    *outfile << word << endl;
  }
  return word;
}

void 
lemur::parse::WriterTextHandler::endDoc() {
  *outfile << "</DOC>" << endl;
}
