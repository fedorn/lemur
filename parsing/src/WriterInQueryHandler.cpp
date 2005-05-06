/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */


#include "WriterInQueryHandler.hpp"
#include <cctype>


lemur::parse::WriterInQueryHandler::WriterInQueryHandler(const string &filename) {
  // create the output file
  outfile = new ofstream(filename.c_str());
  // set state in first doc
  first = true;
}

lemur::parse::WriterInQueryHandler::~WriterInQueryHandler() {
  endDoc();
  // close the output file
  outfile->close();
  delete outfile;
}


char * 
lemur::parse::WriterInQueryHandler::handleDoc(char * docno) {
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
lemur::parse::WriterInQueryHandler::handleWord(char * word) {
  // output the word
  if (word != NULL) {
    *outfile << word << endl;
  }
  return word;
}

char * 
lemur::parse::WriterInQueryHandler::handleSymbol(char * sym) {
  // output the word
  if (sym != NULL) {
    *outfile << sym<< endl;
  }
  return sym;
}

void 
lemur::parse::WriterInQueryHandler::endDoc() {
  *outfile << "</DOC>" << endl;
}
